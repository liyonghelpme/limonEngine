//
// Created by engin-evam on 29.03.2016.
//

#ifndef LIMONENGINE_SDLFONTMANAGER_H
#define LIMONENGINE_SDLFONTMANAGER_H

#include <iostream>
#include <string>
#include <map>

#include <freetype2/ft2build.h>
#include <set>
#include FT_FREETYPE_H
#include "GLHelper.h"


class Glyph {
    GLuint textureID;
    glm::mediump_ivec2 size;
    glm::mediump_ivec2 bearing;
    GLuint advance;
public:
    Glyph(GLHelper *glHelper, FT_Face face, const int size, const char character) :
            textureID(0), size(glm::mediump_vec2(0)), bearing(glm::mediump_vec2(0)), advance(0) {
        //FIXME this is not correct, there is a better function in API
        FT_Set_Pixel_Sizes(face, 0, size);
        if (FT_Load_Char(face, character, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        } else {
            textureID = glHelper->loadTexture(face->glyph->bitmap.rows, face->glyph->bitmap.width, GL_RED,
                                              face->glyph->bitmap.buffer);
            this->size = glm::mediump_ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
            bearing = glm::mediump_ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
            advance = face->glyph->advance.x;
            /**
            std::cout << "Glyph load success. Information:" << std::endl
            << "char: " << character << std::endl
            << "size: " << this->size.x << "," << this->size.y << std::endl
            << "bearing: " << bearing.x << "," << bearing.y << std::endl
            << "advance: " << advance << " (" << advance / 64 << ")" << std::endl;
             */
        }

    }

    GLuint getTextureID() const { return textureID; }

    const glm::mediump_ivec2 &getSize() const { return size; }

    const glm::mediump_ivec2 &getBearing() const { return bearing; }

    GLuint getAdvance() const { return advance; }
};

class Face {
    GLHelper *glHelper;
    std::string path;
    unsigned int size;
    FT_Face face;
    int lineHeight;
    int maxCharWidth;
    std::map<const char, Glyph *> glyphs;
public:
    Face(GLHelper *glHelper, std::string path, int size, FT_Face face) : glHelper(glHelper), path(path), size(size),
                                                                         face(face) {
        lineHeight = face->height;
        maxCharWidth = face->max_advance_width;
    }

    const Glyph *getGlyph(const char character) {
        if (glyphs.count(character) == 0) {
            glyphs[character] = new Glyph(glHelper, face, size, character);
        }
        return glyphs[character];
    }

    int getLineHeight() const {
        return lineHeight;
    }

    int getMaxCharWidth() const {
        return maxCharWidth;
    }

    const std::string &getPath() const {
        return path;
    }

    unsigned int getSize() const {
        return size;
    }

    ~Face() {
        for (std::map<const char, Glyph *>::iterator iter = glyphs.begin(); iter != glyphs.end(); ++iter) {
            delete iter->second;
        }
    }


};

class FontManager {
    GLHelper *glHelper;
    std::map<std::pair<std::string, uint32_t>, Face *> fonts;
    static const std::string DEFAULT_FONT_PATH;
    static const int DEFAULT_FONT_SIZE = 32;
    FT_Library ft;
public:
    explicit FontManager(GLHelper *glHelper);

    Face *getFont(const std::string &fontPath, const uint32_t size);

    std::set<std::pair<std::string, uint32_t>> getLoadedFonts() {
        std::set<std::pair<std::string, uint32_t>> result;
        for(auto it = fonts.begin(); it != fonts.end(); ++it) {
            result.insert(it->first);
        }
        return result;
    }

    ~FontManager();
};


#endif //LIMONENGINE_SDLFONTMANAGER_H
