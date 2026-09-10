// SPDX-License-Identifier: GPL-3.0-or-later

export function normalizePrivateIpv4(value) {
  const input = String(value ?? "").trim();
  const parts = input.split(".");
  if (
    parts.length !== 4 ||
    parts.some((part) => !/^\d{1,3}$/.test(part) || Number(part) > 255)
  ) {
    throw new TypeError("The device IP must be a private-network IPv4 address");
  }

  const octets = parts.map(Number);
  const isPrivate = octets[0] === 10 ||
    (octets[0] === 172 && octets[1] >= 16 && octets[1] <= 31) ||
    (octets[0] === 192 && octets[1] === 168);
  if (!isPrivate) throw new TypeError("The device IP must be a private-network IPv4 address");
  return octets.join(".");
}

export function canonicalProfileUrl(value) {
  let url;
  try {
    url = new URL(String(value ?? "").trim());
  } catch {
    throw new TypeError("A valid Xiaohongshu user profile is required");
  }
  if (
    url.protocol !== "https:" ||
    !["www.xiaohongshu.com", "xiaohongshu.com"].includes(url.hostname) ||
    !url.pathname.startsWith("/user/profile/")
  ) {
    throw new TypeError("A valid Xiaohongshu user profile is required");
  }
  url.search = "";
  url.hash = "";
  return url.toString();
}

export function normalizeBindings(value, { allowIncomplete = false } = {}) {
  if (!Array.isArray(value)) throw new TypeError("Device bindings must be a list");
  const normalized = [];
  const deviceIps = new Set();

  for (const entry of value) {
    if (!entry || typeof entry !== "object" || Array.isArray(entry)) {
      throw new TypeError("Invalid device binding format");
    }
    const rawDeviceIp = String(entry.deviceIp ?? "").trim();
    const rawProfileUrl = String(entry.profileUrl ?? "").trim();
    if (!rawDeviceIp && !rawProfileUrl) continue;
    if (!allowIncomplete && (!rawDeviceIp || !rawProfileUrl)) {
      throw new TypeError("The device IP and the Xiaohongshu profile must both be filled in");
    }

    const deviceIp = rawDeviceIp ? normalizePrivateIpv4(rawDeviceIp) : "";
    const profileUrl = rawProfileUrl ? canonicalProfileUrl(rawProfileUrl) : "";
    if (deviceIp && deviceIps.has(deviceIp)) throw new TypeError("Device IPs must be unique");
    if (deviceIp) deviceIps.add(deviceIp);
    normalized.push({ deviceIp, profileUrl });
  }

  return normalized;
}

export async function migrateBindings(storageArea) {
  const stored = await storageArea.get({ bindings: null, profileUrls: null });
  if (Array.isArray(stored.bindings)) {
    const bindings = normalizeBindings(stored.bindings, { allowIncomplete: true });
    if (JSON.stringify(bindings) !== JSON.stringify(stored.bindings)) {
      await storageArea.set({ bindings });
    }
    if (Array.isArray(stored.profileUrls)) await storageArea.remove("profileUrls");
    return bindings;
  }

  if (Array.isArray(stored.profileUrls)) {
    const bindings = stored.profileUrls
      .map((profileUrl) => ({ deviceIp: "", profileUrl }))
      .filter((binding) => String(binding.profileUrl ?? "").trim())
      .map((binding) => ({
        deviceIp: "",
        profileUrl: canonicalProfileUrl(binding.profileUrl),
      }));
    await storageArea.set({ bindings });
    await storageArea.remove("profileUrls");
    return bindings;
  }

  return [];
}
