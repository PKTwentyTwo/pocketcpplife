// Functions for producing Scalable Vector Graphics:
#pragma once
#include <algorithm>
#include <bitset>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>
#include "gridops.hpp"

#define SVG_MAX 256
// Generate the start of an svg:
std::string svg_head(const int width, const int height, const int dx, const int dy) {
    std::string start = "<!-- svg generated automatically by pocketcpplife -->\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 ";
    start += std::to_string(dx * 8) + " ";
    start += std::to_string(dy * 8) + "\"";
    start += " width=\"" + std::to_string(width) + "px\" ";
    start += " height=\"" + std::to_string(height) + "px\">";
    return start;
}
// Generate SVG for a still life:
std::string svg_still(const ptvec& ptvector, const int width, const int height) {
    const ptvec ptvector2 = defaultshiftgrid(ptvector);
    // Pattern bounding box:
    int32_t* bbox = getgridrect(ptvector2);
    const int x = bbox[0]; const int y = bbox[1]; const int dx = bbox[2]; const int dy = bbox[3];

    int cell_size = 8;
    int real_cell_size = cell_size - 1;
    if (cell_size < 1) {
        cell_size = 1;
    }
    if (real_cell_size < 1) {
        real_cell_size = 1;
    }
    free(bbox);
    std::string graphic = svg_head(width, height, dx, dy);
    
    graphic += std::to_string(width) + " ";
    graphic += std::to_string(height) + "\"";
    graphic += " width=\"" + std::to_string(width) + "px\" ";
    graphic += " height=\"" + std::to_string(height) + "px\">";
    int i;
    // Iterate over each cell and add a rect:
    for (auto i : ptvector) {
        std::string current_rect = "<rect width=\"";
        // Width and height parameters:        
        current_rect += std::to_string(real_cell_size) + "\" ";
        current_rect += "height=\"" + std::to_string(real_cell_size) + "\" ";
        // Position:
        current_rect += "x=\"" + std::to_string(i.first * cell_size) + "\" ";
        current_rect += "y=\"" + std::to_string(i.second * cell_size) + "\" ";
        // Visual stuff:
        current_rect += "rx=\"2\" ry=\"2\" fill=\"black\" opacity=\"1\"";
        // Close the angle brackets and object:
        current_rect += "></rect>";
        graphic += current_rect;
    }
    // Closing tag:
    graphic += "</svg>";
    return graphic;
}
// Generate SVG for an oscillator:
std::string svg_osc(const ptvec& ptvector, const int width, const int height, const int period) {
    // Return still image if period out of range:
    if ((period > SVG_MAX) || (period < 1) || (ptvector.size() == 0)) {
        return svg_still(ptvector, width, height);
    }
    // Stores [x, y, max_x, max_y] instead of [x, y, dx, dy]:
    int32_t bbox_max[4];
    ptvec ptvector2 = ptvector;
    umap<int64_t, std::bitset<SVG_MAX>> cellvalues;
    int i, j;
    // Original bounding box:
    int32_t* bbox2 = getgridrect(ptvector2);
    memcpy(bbox_max, bbox2, 2 * sizeof(int32_t));
    free(bbox2);
    bbox_max[2] += bbox_max[0] - 1;
    bbox_max[3] += bbox_max[1] - 1;
    for (i = 0; i < period; i++) {
        if (i != 1) {
            int32_t* cbbox = getgridrect(ptvector2);
            int32_t x = cbbox[0];
            int32_t y = cbbox[1];
            int32_t max_x = cbbox[2] + x - 1;
            int32_t max_y = cbbox[3] + y - 1;
            if (x < bbox_max[0]) {
                bbox_max[0] = x;
            }
            if (y < bbox_max[1]) {
                bbox_max[1] = y;
            }
            if (max_x > bbox_max[2]) {
                bbox_max[2] = max_x;
            }
            if (max_y > bbox_max[3]) {
                bbox_max[3] = max_y;
            }
            free(cbbox);
        }
        // Store cell values:
        for (auto j : ptvector2) {
            int64_t key = tokey(j.first, j.second);
            cellvalues[key][i] = 1;
        }
        cppadvance(ptvector2, 1);
    }
    // Calculate the effective bounding box:
    int32_t bbox[4];
    bbox[0] = bbox_max[0];
    bbox[1] = bbox_max[1];
    bbox[2] = bbox_max[2] - bbox_max[0] + 1;
    bbox[3] = bbox_max[3] - bbox_max[1] + 1;
    // Finally, we can actually start putting together the SVG:
    const int32_t x = bbox[0];
    const int32_t y = bbox[1];
    const int32_t dx = bbox[2];
    const int32_t dy = bbox[3];
    int cell_size = 8;
    int real_cell_size = cell_size - 1;
    if (cell_size < 1) {
        cell_size = 1;
    }
    if (real_cell_size < 1) {
        real_cell_size = 1;
    }
    std::string graphic = svg_head(width, height, dx, dy);
    
    graphic += std::to_string(width) + " ";
    graphic += std::to_string(height) + "\"";
    graphic += " width=\"" + std::to_string(width) + "px\" ";
    graphic += " height=\"" + std::to_string(height) + "px\">";
    // Iterate over each cell and add a rect:
    for (auto i : cellvalues) {
        int64_t key = i.first;
        std::bitset<SVG_MAX> value = i.second;
        std::pair<int32_t, int32_t> coord = std::make_pair(getx(key), gety(key));
        int32_t effectivex = coord.first - x;
        int32_t effectivey = coord.second - y;
        std::string current_rect = "<rect width=\"";
        // Width and height parameters:        
        current_rect += std::to_string(real_cell_size) + "\" ";
        current_rect += "height=\"" + std::to_string(real_cell_size) + "\" ";
        // Position:
        current_rect += "x=\"" + std::to_string(effectivex * cell_size) + "\" ";
        current_rect += "y=\"" + std::to_string(effectivey * cell_size) + "\" ";
        // Visual stuff:
        current_rect += "rx=\"2\" ry=\"2\" fill=\"black\" opacity=\"";
        if (value[0]) {
            current_rect += "1\"";
        }
        else {
            current_rect += "0.3\"";
        }
        // Close the angle brackets and object:
        current_rect += ">";
        // Animation example:
        // <animate attributeName="opacity" values="1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1" dur="2s" repeatCount="indefinite"/>
        current_rect += "<animate attributeName=\"opacity\" values=\"";
        for (j = 0; j < period; j++) {
            if (value[j]) {
                current_rect += "1";
            }
            else {
                current_rect += "0.3";
            }
            if ((j + 1) < period) {
                current_rect += ";";
            }
        }
        current_rect += "\" dur=\"2s\" repeatCount=\"indefinite\"/>";
        current_rect += "</rect>";
        graphic += current_rect;
    }
    // Closing tag:
    graphic += "</svg>";
    return graphic;
}
// Generate SVG for a spaceship:
std::string svg_ship(const ptvec& ptvector, const int width, const int height, const int period, const int32_t pattern_dx, const int32_t pattern_dy) {
    // Return still image if period out of range:
    if ((period > SVG_MAX) || (period < 1) || (ptvector.size() == 0)) {
        return svg_still(ptvector, width, height);
    }
    // Stores [x, y, max_x, max_y] instead of [x, y, dx, dy]:
    int32_t bbox_max[4];
    ptvec ptvector2 = ptvector;
    umap<int64_t, std::bitset<SVG_MAX>> cellvalues;
    int i, j;
    // Original bounding box:
    int32_t* bbox2 = getgridrect(ptvector2);
    memcpy(bbox_max, bbox2, 2 * sizeof(int32_t));
    free(bbox2);
    bbox_max[2] += bbox_max[0] - 1;
    bbox_max[3] += bbox_max[1] - 1;
    for (i = 0; i < period; i++) {
        if (i != 1) {
            int32_t* cbbox = getgridrect(ptvector2);
            int32_t x = cbbox[0];
            int32_t y = cbbox[1];
            int32_t max_x = cbbox[2] + x - 1;
            int32_t max_y = cbbox[3] + y - 1;
            if (x < bbox_max[0]) {
                bbox_max[0] = x;
            }
            if (y < bbox_max[1]) {
                bbox_max[1] = y;
            }
            if (max_x > bbox_max[2]) {
                bbox_max[2] = max_x;
            }
            if (max_y > bbox_max[3]) {
                bbox_max[3] = max_y;
            }
            free(cbbox);
        }
        // Store cell values:
        for (auto j : ptvector2) {
            int64_t key = tokey(j.first, j.second);
            cellvalues[key][i] = 1;
        }
        cppadvance(ptvector2, 1);
    }
    // Calculate the effective bounding box:
    int32_t bbox[4];
    bbox[0] = bbox_max[0];
    bbox[1] = bbox_max[1];
    bbox[2] = bbox_max[2] - bbox_max[0] + 1;
    bbox[3] = bbox_max[3] - bbox_max[1] + 1;
    // Finally, we can actually start putting together the SVG:
    int32_t x = bbox[0];
    int32_t y = bbox[1];
    int32_t dx = bbox[2];
    int32_t dy = bbox[3];
    if (pattern_dx < 0) {
        dx += pattern_dx;
    }
    else {
        x -= pattern_dx;
    }
    if (pattern_dy < 0) {
        dy += pattern_dy;
    }
    else {
        y -= pattern_dy;
    }
    // Ensure we have a boundary-ish thingy:
    x--;
    y--;
    dx++;
    dy++;
    int cell_size = 8;
    int real_cell_size = cell_size - 1;
    if (cell_size < 1) {
        cell_size = 1;
    }
    if (real_cell_size < 1) {
        real_cell_size = 1;
    }
    std::string graphic = svg_head(width, height, dx, dy);
    
    graphic += std::to_string(width) + " ";
    graphic += std::to_string(height) + "\"";
    graphic += " width=\"" + std::to_string(width) + "px\" ";
    graphic += " height=\"" + std::to_string(height) + "px\">";
    // Iterate over each cell and add a rect:
    for (auto i : cellvalues) {
        int64_t key = i.first;
        std::bitset<SVG_MAX> value = i.second;
        std::pair<int32_t, int32_t> coord = std::make_pair(getx(key), gety(key));
        int32_t effectivex = coord.first - x;
        int32_t effectivey = coord.second - y;
        std::string current_rect = "<rect width=\"";
        // Width and height parameters:        
        current_rect += std::to_string(real_cell_size) + "\" ";
        current_rect += "height=\"" + std::to_string(real_cell_size) + "\" ";
        // Position:
        current_rect += "x=\"" + std::to_string(effectivex * cell_size) + "\" ";
        current_rect += "y=\"" + std::to_string(effectivey * cell_size) + "\" ";
        // Visual stuff:
        current_rect += "rx=\"2\" ry=\"2\" fill=\"black\" opacity=\"";
        if (value[0]) {
            current_rect += "1\"";
        }
        else {
            current_rect += "0\"";
        }
        // Close the angle brackets and object:
        current_rect += ">";
        /* Animation example:
           <animate attributeName="opacity" values="0;0;0;0;0;0;0;0;0;0;0;1;1;1;1" dur="2s" repeatCount="indefinite"/>
           <animate attributeName="y" values="80;72" dur="2s" repeatCount="indefinite"/>
        */
        current_rect += "<animate attributeName=\"opacity\" values=\"";
        for (j = 0; j < period; j++) {
            if (value[j]) {
                current_rect += "1";
            }
            else {
                current_rect += "0";
            }
            if ((j + 1) < period) {
                current_rect += ";";
            }
        }
        current_rect += "\" dur=\"2s\" repeatCount=\"indefinite\"/>";
        // Spaceship movement
        if (pattern_dx != 0) {
            current_rect += "<animate attributeName=\"x\" values=\"";
            current_rect += std::to_string(effectivex * cell_size) + ";" + std::to_string((effectivex - pattern_dx) * cell_size) + "\"";
            current_rect += " dur=\"2s\" repeatCount=\"indefinite\"/>";
        }
        if (pattern_dy != 0) {
            current_rect += "<animate attributeName=\"y\" values=\"";
            current_rect += std::to_string(effectivey * cell_size) + ";" + std::to_string((effectivey - pattern_dy) * cell_size) + "\"";
            current_rect += " dur=\"2s\" repeatCount=\"indefinite\"/>";
        }
        current_rect += "</rect>";
        graphic += current_rect;
    }
    // Closing tag:
    graphic += "</svg>";
    return graphic;
}

