#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include "structs.h"

#if (defined(_WIN32) || defined(__WIN32__))
#define mkdir(A, B) mkdir(A)
#endif

#define readT(__TYPE__, __VAR__, __FILE__) \
    __TYPE__ __VAR__;\
    if (fread(&__VAR__, sizeof(__TYPE__), 1, __FILE__) != 1) {\
        fprintf(stderr, "failed to read file\n");\
    }\

#define readTarray(__TYPE__, __VAR__, __NUM__, __FILE__) \
    __TYPE__ __VAR__[__NUM__];\
    if (fread(__VAR__, sizeof(__TYPE__), __NUM__, __FILE__) != __NUM__) {\
        fprintf(stderr, "failed to read file\n");\
    }\

#define readByte(__VAR__, __FILE__) \
    readT(char, __VAR__, __FILE__)

#define readBytes(__VAR__, __NUM__, __FILE__) \
    readTarray(char, __VAR__, __NUM__, __FILE__)

#define readBytesA(__VAR__, __SIZE__, __FILE__) \
    char *__VAR__ = (char *)malloc(__SIZE__);\
    if (fread(__VAR__, sizeof(char), __SIZE__, __FILE__) != __SIZE__) {\
        fprintf(stderr, "failed to read file\n");\
    }\

#define read32(__VAR__, __FILE__) \
    readT(uint32_t, __VAR__, __FILE__)

#define read32array(__VAR__, __NUM__, __FILE__) \
    readTarray(uint32_t, __VAR__, __NUM__, __FILE__)

#define openFile(__VAR__, __OPT__, __FORMAT__, ...) \
    FILE *__VAR__;\
    {\
        char filename[100];\
        sprintf(filename, __FORMAT__, __VA_ARGS__);\
        __VAR__ = fopen(filename, __OPT__);\
    }

#define writeToFile(__BYTES__, __SIZE__, __FORMAT__, ...) {\
    openFile(file, "wb", __FORMAT__, __VA_ARGS__);\
    if (fwrite(__BYTES__, __SIZE__, 1, file) != 1) {\
        fprintf(stderr, "failed to write file\n");\
    }\
    fclose(file);\
}\

#define copyToFile(__FILE__, __SIZE__, __FORMAT__, ...) {\
    char *bytes = (char *)malloc(__SIZE__);\
    if (fread(bytes, sizeof(char), __SIZE__, __FILE__) != __SIZE__) {\
        fprintf(stderr, "failed to read file\n");\
    }\
    writeToFile(bytes, __SIZE__, __FORMAT__, __VA_ARGS__);\
    free(bytes);\
}

// the caller must free() the returnd string
#define readStringAt(__VAR__, __OFFSET__, __FILE__) \
    char __VAR__[10000];\
    fseek(__FILE__, __OFFSET__, SEEK_SET);\
    fscanf(__FILE__, "%s", __VAR__);

#define readList(__SIZE_VAR__, __ARRAY_VAR__, __FILE__) \
    read32(__SIZE_VAR__, __FILE__);\
    read32array(__ARRAY_VAR__, __SIZE_VAR__, __FILE__);

#define readListObjects(__TYPE__, __VAR__, __FILE__) \
    readList(__VAR__ ## Size, __VAR__ ## Offsets, __FILE__);\
    readTarray(__TYPE__, __VAR__, __VAR__ ## Size, __FILE__);

static void usage(void) {
    fprintf(stderr, "usage: gmspack [-ae] [file]\n");
    exit(1);
}

#define dumpT(__DATA__, __FORMAT__, __SIZE__, __WRAP__) {\
    fprintf(stdout, "========");\
    for (long i = 0; i < __SIZE__; i++) {\
        if (i % __WRAP__ == 0) {\
            fprintf(stdout, "\n");\
        }\
        fprintf(stdout, __FORMAT__, __DATA__[i]);\
    }\
    fprintf(stdout, "\n========\n");\
}

#define cmkdir(__NAME__) \
    mkdir(__NAME__, 0755);\
    chmod(__NAME__, 0755);\
    chdir(__NAME__);

static void dump32uint(uint32_t *values, long size, int wrap) {
    dumpT(values, "%x\t", size, wrap);
}

static void dumpFloat(float *values, long size, int wrap) {
    dumpT(values, "%f\t", size, wrap);
}

int main(int argc, char *argv[]) {
    int ch;
    while ((ch = getopt(argc, argv, "a:e:")) != -1) {
        switch (ch) {
        case 'a':
            fprintf(stdout, "%s %s\n", optarg, argv[1]);
            break;
        case 'e': {
            FILE *file = fopen(optarg, "rb");
            if (!file) {
                fprintf(stderr, "cannot open file\n");
                exit(1);
            }
            cmkdir("output");

            mkdir("chunk", 0755);
            chmod("chunk", 0755);

            while (1) {
                Chunk chunk;
                if (fread(&chunk, sizeof(Chunk), 1, file) != 1) {
                    break;
                }

                // add string terminater
                char chunkName[5];
                memcpy(chunkName, chunk.name, 4);
                chunkName[4] = '\0';
                long chunkTop = ftell(file);
                long chunkLast = chunkTop + chunk.size;
                fprintf(stdout, "%s (%u KB @ %u): ", chunkName, chunk.size / 1024, chunkTop);

                if (strcmp(chunkName, "FORM") == 0) {
                    uint32_t totalSize = chunk.size;
                    fprintf(stdout, "%n");
                } else {
                    if (strcmp(chunkName, "STRG") == 0) {
                        FILE *stringFile = fopen("string.txt", "wb");

                        readList(entryNum, entryOffsets, file);
                        fprintf(stdout, "%u texts\n", entryNum);

                        for (int i = 0; i < entryNum; i++) {
                            read32(entrySize, file);

                            readBytes(itemBuf, entrySize, file);
                            fwrite(itemBuf, entrySize, 1, stringFile);

                            fputs("\n", stringFile);
                            fseek(file, 1, SEEK_CUR); // 1 byte margin between strings 
                        }

                        fclose(stringFile);
                        fseek(file, chunkLast, SEEK_SET);
                    } else if (strcmp(chunkName, "TXTR") == 0) {
                        cmkdir("texture");

                        readList(entryNum, entryOffsets, file);
                        fprintf(stdout, "%u texture files\n", entryNum);

                        readTarray(TextureAddress, addresses, entryNum, file);

                        for (int i = 0; i < entryNum; i++) {
                            uint32_t nextOffset = i < entryNum - 1 ? addresses[i + 1].offset : chunkLast;
                            uint32_t entrySize = nextOffset - addresses[i].offset;
                            copyToFile(file, entrySize, "%d.png", i);
                        }

                        chdir("..");
                        fseek(file, chunkLast, SEEK_SET);
                    } else if (strcmp(chunkName, "AUDO") == 0) {
                        cmkdir("audio");

                        readList(entryNum, entryOffsets, file);
                        fprintf(stdout, "%u audio files\n", entryNum);

                        for (int i = 0; i < entryNum; i++) {
                            fseek(file, entryOffsets[i], SEEK_SET);
                            read32(entrySize, file);
                            copyToFile(file, entrySize, "%d.wav", i);
                        }

                        chdir("..");
                        fseek(file, chunkLast, SEEK_SET);
                    } else if (strcmp(chunkName, "SPRT") == 0) {
                        FILE *spriteFile = fopen("sprite.csv", "wb");

                        readList(entryNum, entryOffsets, file);
                        fprintf(stdout, "%u sprites\n", entryNum);

                        SpritePrintCSVHeader(spriteFile);

                        for (int i = 0; i < entryNum; i++) {
                            fseek(file, entryOffsets[i], SEEK_SET);
                            readT(Sprite, sprite, file);
                            readList(textureCount, textureAddresses, file);
                            readStringAt(spriteName, sprite.nameOffset, file);

                            SpritePrintCSV(spriteFile, sprite, spriteName);
                        }

                        fclose(spriteFile);
                        fseek(file, chunkLast, SEEK_SET);
                    } else if (strcmp(chunkName, "FONT") == 0) {
                        cmkdir("fonts");

                        readList(entryNum, entryOffsets, file);
                        fprintf(stdout, "%u fonts\n", entryNum);

                        for (int i = 0; i < entryNum; i++) {
                            fseek(file, entryOffsets[i], SEEK_SET);
                            readT(Font, font, file);
                            readList(glyphCount, glyphOffsets, file);
                            readTarray(Glyph, glyphs, glyphCount, file);
                            
                            readStringAt(fileName, font.fileNameOffset, file);
                            readStringAt(name, font.nameOffset, file);

                            openFile(fontFile, "wb", "%s.font.gmx", fileName)
                            writeFontXML(fontFile, font, name, glyphs, glyphCount);
                            fclose(fontFile);
                        }

                        chdir("..");
                        fseek(file, chunkLast, SEEK_SET);
                    } else if (strcmp(chunkName, "ROOM") == 0) {
                        cmkdir("rooms");

                        readList(entryNum, entryOffsets, file);
                        fprintf(stdout, "%u rooms\n", entryNum);

                        for (int i = 0; i < entryNum; i++) {
                            fseek(file, entryOffsets[i], SEEK_SET);
                            readT(Room, room, file);

                            readListObjects(Background, backgrounds, file);
                            readListObjects(View, views, file);
                            readListObjects(RoomObject, objects, file);
                            readListObjects(Tile, tiles, file);

                            readStringAt(name, room.nameOffset, file);
                            readStringAt(caption, room.captionOffset, file);

                            openFile(roomFile, "wb", "%s.room.gmx", name);
                            writeRoomXML(roomFile, room, caption, 
                                backgrounds, NULL, backgroundsSize, 
                                views, NULL, viewsSize,
                                objects, NULL, objectsSize,
                                tiles, tilesSize);
                            fclose(roomFile);

                            RoomPrintCSV(roomFile, room, name, caption);
                        }

                        chdir("..");
                        fseek(file, chunkLast, SEEK_SET);
                    } else if (strcmp(chunkName, "SOND") == 0) {
                        FILE *soundFile = fopen("sound.csv", "wb");

                        readList(entryNum, entryOffsets, file);
                        fprintf(stdout, "%u sounds\n", entryNum);

                        SoundPrintCSVHeader(soundFile);

                        for (int i = 0; i < entryNum; i++) {
                            fseek(file, entryOffsets[i], SEEK_SET);
                            readT(Sound, sound, file);
                            readStringAt(name, sound.nameOffset, file);
                            readStringAt(fileType, sound.fileTypeOffset, file);
                            readStringAt(fileName, sound.fileNameOffset, file);

                            SoundPrintCSV(soundFile, sound, name, fileType, fileName);
                        }

                        fclose(soundFile);
                        fseek(file, chunkLast, SEEK_SET);
                    } else if (strcmp(chunkName, "BGND") == 0) {
                        FILE *bgFile = fopen("background.csv", "wb");

                        readList(entryNum, entryOffsets, file);
                        fprintf(stdout, "%u backgrounds\n", entryNum);

                        BackgroundDefinitionPrintCSVHeader(bgFile);

                        for (int i = 0; i < entryNum; i++) {
                            fseek(file, entryOffsets[i], SEEK_SET);
                            readT(BackgroundDefinition, bg, file);
                            readStringAt(name, bg.nameOffset, file);

                            BackgroundDefinitionPrintCSV(bgFile, bg, name);
                        }

                        fclose(bgFile);
                        fseek(file, chunkLast, SEEK_SET);
                    } else if (strcmp(chunkName, "OBJT") == 0) {
                        FILE *objFile = fopen("object.csv", "wb");

                        readList(entryNum, entryOffsets, file);
                        fprintf(stdout, "%u objects\n", entryNum);

                        GameObjectPrintCSVHeader(objFile);

                        for (int i = 0; i < entryNum; i++) {
                            fseek(file, entryOffsets[i], SEEK_SET);
                            readT(GameObject, obj, file);
                            readStringAt(name, obj.nameOffset, file);

                            GameObjectPrintCSV(objFile, obj, name);
                        }

                        fclose(objFile);
                        fseek(file, chunkLast, SEEK_SET);
                    } else if (strcmp(chunkName, "PATH") == 0) {
                        readList(entryNum, entryOffsets, file);
                        fprintf(stdout, "%u paths\n", entryNum);

                        FILE *pathFile = fopen("path.csv", "wb");
                        FILE *pointFile = fopen("path-point.csv", "wb");

                        PathPrintCSVHeader(pathFile);
                        PathPointPrintCSVHeader(pointFile);

                        for (int i = 0; i < entryNum; i++) {
                            fseek(file, entryOffsets[i], SEEK_SET);
                            readT(Path, path, file);

                            read32(pointCount, file);
                            readTarray(PathPoint, points, pointCount, file);
                            for (int n = 0; n < pointCount; n++) {
                                PathPointPrintCSV(pointFile, points[n], i);
                            }
                            
                            readStringAt(name, path.nameOffset, file);
                            PathPrintCSV(pathFile, path, name);
                        }

                        fclose(pathFile);
                        fclose(pointFile);
                        fseek(file, chunkLast, SEEK_SET);
                    } else if (strcmp(chunkName, "SCPT") == 0) {
                        readList(entryNum, entryOffsets, file);
                        fprintf(stdout, "%u scripts\n", entryNum);

                        FILE *scriptFile = fopen("script.csv", "wb");

                        ScriptDefinitionPrintCSVHeader(scriptFile);

                        readTarray(ScriptDefinition, scripts, entryNum, file);

                        for (int i = 0; i < entryNum; i++) {
                            readStringAt(name, scripts[i].nameOffset, file);
                            ScriptDefinitionPrintCSV(scriptFile, scripts[i], name);
                        }

                        fclose(scriptFile);
                        fseek(file, chunkLast, SEEK_SET);
                    } else {
                        chdir("./chunk");
                        copyToFile(file, chunk.size, "%s.chunk", chunkName);
                        chdir("..");

                        fprintf(stdout, "skip.\n");
                    }
                }
            }
            break;
        }
        default:
            usage();
        }
    }
}
