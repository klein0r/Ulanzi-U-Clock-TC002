// SPDX-License-Identifier: GPL-3.0-or-later
import { migrateRefreshSeconds, normalizeRefreshSeconds } from "./refresh-config.js";
import { canonicalProfileUrl, migrateBindings, normalizeBindings } from "./bindings-config.js";
import {
  migrateHomeAssistantConfig,
  normalizeHomeAssistantUrl,
  normalizeWebhookId,
  requiredOrigins,
} from "./ha-config.js";

const DEFAULTS = {
  lastResults: {},
};

document.addEventListener("DOMContentLoaded", restore);
document.querySelector("#save").addEventListener("click", save);
document.querySelector("#addBinding").addEventListener("click", () => addBindingRow());

async function restore() {
  const [stored, haConfig, refreshSeconds, bindings] = await Promise.all([
    chrome.storage.local.get(DEFAULTS),
    migrateHomeAssistantConfig(chrome.storage.local),
    migrateRefreshSeconds(chrome.storage.local),
    migrateBindings(chrome.storage.local),
  ]);
  renderBindings(bindings.length ? bindings : [{ deviceIp: "", profileUrl: "" }]);
  document.querySelector("#refreshSeconds").value = refreshSeconds;
  document.querySelector("#homeAssistantUrl").value = haConfig.homeAssistantUrl;
  document.querySelector("#webhookId").value = haConfig.webhookId;
  document.querySelector("#lastResult").textContent = Object.keys(stored.lastResults).length
    ? `Latest result per device (including the dynamically discovered devicePrefix): ${JSON.stringify(stored.lastResults, null, 2)}`
    : "No results yet";
  await showPermissionStatus(haConfig.homeAssistantUrl, bindings);
}

async function save() {
  const status = document.querySelector("#status");
  try {
    const bindings = normalizeBindings([...document.querySelectorAll(".binding")].map((row) => ({
      deviceIp: row.querySelector("[data-field=deviceIp]").value,
      profileUrl: canonicalProfileUrl(row.querySelector("[data-field=profileUrl]").value),
    })));
    if (!bindings.length) throw new Error("Add at least one device");

    const refreshSeconds = normalizeRefreshSeconds(document.querySelector("#refreshSeconds").value);
    const homeAssistantUrl = normalizeHomeAssistantUrl(document.querySelector("#homeAssistantUrl").value);
    const webhookId = normalizeWebhookId(document.querySelector("#webhookId").value);
    const origins = requiredOrigins(homeAssistantUrl, bindings);
    const permission = { origins };
    const granted = await chrome.permissions.contains(permission)
      || await chrome.permissions.request(permission);
    if (!granted) throw new Error("Permission to access the Home Assistant and TC002 addresses is required");

    await chrome.storage.local.set({ bindings, refreshSeconds, homeAssistantUrl, webhookId });
    await chrome.storage.local.remove(["profileUrls", "bridgeUrl", "bridgeToken"]);
    renderBindings(bindings);
    await showPermissionStatus(homeAssistantUrl, bindings);
    status.textContent = "Saved; it will refresh within a few seconds";
  } catch (error) {
    status.textContent = error.message;
  }
}

function renderBindings(bindings) {
  const container = document.querySelector("#bindings");
  container.replaceChildren();
  for (const binding of bindings) addBindingRow(binding);
  const incomplete = bindings.some((binding) => !binding.deviceIp || !binding.profileUrl);
  document.querySelector("#bindingWarning").textContent = incomplete
    ? "The old profile configuration was migrated; add a TC002 device IP for each profile and save."
    : "";
}

function addBindingRow(binding = { deviceIp: "", profileUrl: "" }) {
  const row = document.createElement("div");
  row.className = "binding";
  row.append(
    field("TC002 device IP", "deviceIp", binding.deviceIp, "The TC002's private-network IPv4"),
    field("Xiaohongshu profile URL", "profileUrl", binding.profileUrl, "https://www.xiaohongshu.com/user/profile/..."),
  );
  const remove = document.createElement("button");
  remove.type = "button";
  remove.textContent = "Remove";
  remove.addEventListener("click", () => row.remove());
  row.append(remove);
  document.querySelector("#bindings").append(row);
}

function field(labelText, name, value, placeholder) {
  const label = document.createElement("label");
  label.textContent = labelText;
  const input = document.createElement("input");
  input.dataset.field = name;
  input.value = value;
  input.placeholder = placeholder;
  input.autocomplete = "off";
  label.append(input);
  return label;
}

async function showPermissionStatus(homeAssistantUrl, bindings) {
  const target = document.querySelector("#permissionStatus");
  if (!homeAssistantUrl || !bindings.some((binding) => binding.deviceIp)) {
    target.textContent = "Saving will request permission to access Home Assistant and the TC002 addresses you entered.";
    return;
  }
  try {
    const origins = requiredOrigins(homeAssistantUrl, bindings.filter((binding) => binding.deviceIp));
    const granted = await chrome.permissions.contains({ origins });
    target.textContent = granted ? "Local network access has been granted." : "Not all local network permissions have been granted; please save again.";
  } catch {
    target.textContent = "Enter a valid configuration and save.";
  }
}
