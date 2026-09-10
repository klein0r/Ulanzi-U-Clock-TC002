#ifndef PAINTER_H_
#define PAINTER_H_

#include "Surface.h"
#include <string>
#include <vector>
#include "Fonts/TomThumb.h"
class Painter {
public:
    static Painter& getInstance();
    
    /**
     * Draw a rectangle
     * @param surface The drawing surface
     * @param x x coordinate of the top-left corner
     * @param y y coordinate of the top-left corner
     * @param width Rectangle width
     * @param height Rectangle height
     * @param color Color
     * @param filled Whether to fill (true=filled, false=single-pixel outline only)
     */
    void drawRect(Surface& surface, int x, int y, int width, int height, 
                  const Color& color, bool filled = true);
    

    /**
     * Draw a point
     * @param surface The drawing surface
     * @param x x coordinate
     * @param y y coordinate
     * @param color Color
     */
    void drawPixel(Surface& surface, int x, int y, const Color& color);

    /**
     * Draw a line (Bresenham)
     * @param surface The drawing surface
     * @param x0 Start x
     * @param y0 Start y
     * @param x1 End x
     * @param y1 End y
     * @param color Color
     */
    void drawLine(Surface& surface, int x0, int y0, int x1, int y1, const Color& color);

    /**
     * Draw a circle
     * @param surface The drawing surface
     * @param cx Center x coordinate
     * @param cy Center y coordinate
     * @param cr Circle radius
     * @param color Color
     * @param filled Whether to fill (true=filled, false=outline only)
     */
    void drawCircle(Surface& surface, int cx, int cy, int cr, 
                    const Color& color, bool filled = false);

    /**
     * Draw an RGB888 bitmap
     * @param surface The drawing surface
     * @param x x coordinate of the top-left corner
     * @param y y coordinate of the top-left corner
     * @param w Bitmap width
     * @param h Bitmap height
     * @param data RGB888 pixel array; each element is a uint32_t in the format 0x00RRGGBB, length = w*h
     */
    void drawBitmap(Surface& surface, int x, int y, int w, int h,
                    const std::vector<uint32_t>& data);

    /**
     * Draw an image
     * @param surface The drawing surface
     * @param imagePath Image path
     * @param x x coordinate of the top-left corner
     * @param y y coordinate of the top-left corner
     */
    void drawImage(Surface& surface, const std::string& imagePath, int x, int y);
    
    /**
     * Draw an image centered within the given area
     * @param surface The drawing surface
     * @param imagePath Image path
     * @param rectX x coordinate of the area's top-left corner
     * @param rectY y coordinate of the area's top-left corner
     * @param rectWidth Area width
     * @param rectHeight Area height
     */
    void drawImageCentered(Surface& surface, const std::string& imagePath, 
                          int rectX, int rectY, int rectWidth, int rectHeight);
    
    /**
     * Draw a single character (using the TomThumb font)
     * @param surface The drawing surface
     * @param x x coordinate of the top-left corner
     * @param y y coordinate of the top-left corner (baseline position)
     * @param c The character to draw
     * @param color Color
     */
    void drawChar(Surface& surface, int x, int y, char c, const Color& color);
    
    /**
     * Draw a string (using the TomThumb font)
     * @param surface The drawing surface
     * @param x x coordinate of the top-left corner
     * @param y y coordinate of the top-left corner (baseline position)
     * @param text The string to draw
     * @param color Color
     * @param letterSpacing Character spacing (1 pixel by default)
     */
    void drawText(Surface& surface, int x, int y, const std::string& text, 
                  const Color& color, int letterSpacing = 1);
    
    /**
     * Get the width of a string (in pixels)
     * @param text The string
     * @param letterSpacing Character spacing
     * @return The total width of the string
     */
    int getTextWidth(const std::string& text, int letterSpacing = 1);

private:
    Painter();
    ~Painter();
    Painter(const Painter&) = delete;
    Painter& operator=(const Painter&) = delete;
};

#endif
