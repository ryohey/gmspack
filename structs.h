typedef struct {
    int32_t width, height;
} Size;

typedef struct {
    int32_t x, y;
} Point;

typedef struct {
    uint32_t left, right, bottom, top;
} Rect;

typedef struct {
    float x, y;
} PointF;

typedef struct {
    char name[4];
    uint32_t size;
} Chunk;

typedef struct {
    uint32_t padding;
    uint32_t offset;
} TextureAddress;

typedef struct {
    uint32_t nameOffset;
    Size size;
    Rect bounds;
    uint32_t boundingBoxMode;
    uint32_t isSeparateColMasks;
    Point position;
} Sprite;

extern void SpritePrintCSVHeader(FILE *file) {
    fprintf(file, "name,width,height,left,top,right,bottom,boundingBoxMode,isSeparateColMasks,x,y\n");
}

extern void SpritePrintCSV(FILE *file, Sprite s, char *name) {
    fprintf(file, "\"%s\",%u,%u,%d,%d,%d,%d,%u,%u,%d,%d\n", 
        name, 
        s.size.width, s.size.height, 
        s.bounds.left, s.bounds.top, s.bounds.right, s.bounds.bottom, 
        s.boundingBoxMode, s.isSeparateColMasks, s.position.x, s.position.y);
}

typedef struct {
    uint32_t fileNameOffset;
    uint32_t nameOffset;
    int32_t pointSize;
    int32_t isBold;
    int32_t isItalic;
    char antiAliasMode;
    char charset;
    int32_t texturePageId;
    PointF scale;
    uint16_t unknown;
} Font;

extern void FontPrintCSVHeader(FILE *file) {
    fprintf(file, "fileName,name,pointSize,isBold,isItalic,antiAliasMode,charset,texturePageId,scaleX,scaleY,unknown\n");
}

extern void FontPrintCSV(FILE *file, Font f, char *fileName, char *name) {
    fprintf(file, "\"%s\",\"%s\",%u,%u,%u,%u,%u,%d,%f,%f,%u\n", 
        fileName, name, f.pointSize, f.isBold, f.isItalic, f.antiAliasMode, f.charset, f.texturePageId, f.scale.x, f.scale.y, f.unknown);
}

typedef struct {
    uint8_t keyCode;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t shift;
    uint32_t offset;
} Glyph;

extern void GlyphPrintCSVHeader(FILE *file) {
    fprintf(file, "keyCode,x,y,width,height,shift,offset\n");
}

extern void GlyphPrintCSV(FILE *file, Glyph g) {
    fprintf(file, "%u,%u,%u,%u,%u,%u,%u\n", g.keyCode, g.x, g.y, g.width, g.height, g.shift, g.offset);
}

#define XMLBool(__VAR__) (__VAR__ ? -1 : 0)

extern void writeFontXML(FILE *file, Font f, char *fontName, Glyph *glyphs, int glyphCount) {
    fprintf(file, "<font>\n");
    fprintf(file, "\t<name>%s</name>\n", fontName);
    fprintf(file, "\t<size>%d</size>\n", f.pointSize);
    fprintf(file, "\t<bold>%d</bold>\n", XMLBool(f.isBold));
    fprintf(file, "\t<italic>%d</italic>\n", XMLBool(f.isItalic));

    fprintf(file, "\t<glyphs>\n");
    for (int i = 0; i < glyphCount; i++) {
        Glyph g = glyphs[i];
        fprintf(file, "\t\t<glyph character=\"%u\" x=\"%u\" y=\"%u\" w=\"%u\" h=\"%u\" shift=\"%u\" offset=\"%u\"/>\n",
            g.keyCode, g.x, g.y, g.width, g.height, g.shift, g.offset
            );
    }
    fprintf(file, "\t</glyphs>\n");

    // unsupported 
    fprintf(file, "\t<kerningPairs/>\n");
    fprintf(file, "\t<image>%s</image>\n", "");
    fprintf(file, "\t<charset>%d</charset>\n", 1);
    fprintf(file, "\t<aa>%d</aa>\n", 0);
    fprintf(file, "\t<includeTTF>%d</includeTTF>\n", 0);
    fprintf(file, "\t<TTFName>%s</TTFName>\n", "");
    fprintf(file, "\t<texgroups><texgroup0>0</texgroup0></texgroups>\n");
    fprintf(file, "\t<ranges><range0>32,127</range0></ranges>\n");
    fprintf(file, "</font>\n");
}

typedef struct {
    uint32_t nameOffset;
    uint32_t unknown[3];
    uint32_t textureAddress;
} BackgroundDefinition;

extern void BackgroundDefinitionPrintCSVHeader(FILE *file) {
    fprintf(file, "name,textureAddress\n");
}

extern void BackgroundDefinitionPrintCSV(FILE *file, BackgroundDefinition b, char *name) {
    fprintf(file, "%s,%u\n", name, b.textureAddress);
}

typedef struct {
    uint32_t isEnabled;
    uint32_t isForeground;
    int32_t id;
    Point position;
    uint32_t isTileX;
    uint32_t isTileY;
    Point speed;
    uint32_t isStretch;
} Background;

extern void BackgroundPrintCSVHeader(FILE *file) {
    fprintf(file, "room,isEnabled,isForeground,id,positionX,positionY,isTileX,isTileY,speedX,speedY,isStretch\n");
}

extern void BackgroundPrintCSV(FILE *file, Background b, int room) {
    fprintf(file, "%d,%u,%u,%d,%d,%d,%u,%u,%d,%d,%u\n", 
        room,
        b.isEnabled, b.isForeground, b.id, 
        b.position.x, b.position.y, 
        b.isTileX, b.isTileY,
        b.speed.x, b.speed.y,
        b.isStretch);
}

typedef struct {
    uint32_t nameOffset;
    int32_t id;
} ScriptDefinition;

extern void ScriptDefinitionPrintCSVHeader(FILE *file) {
    fprintf(file, "name,id\n");
}

extern void ScriptDefinitionPrintCSV(FILE *file, ScriptDefinition d, char *name) {
    fprintf(file, "%s,%d\n", name, d.id);
}

typedef struct {
    uint32_t nameOffset;
    uint32_t spriteIndex;
} GameObjectDefinition;

typedef struct {
    uint32_t nameOffset;
    uint32_t captionOffset;
    Size size;
    uint32_t speed;
    uint32_t isPersistent;
    uint32_t color; // argb
    uint32_t isDrawBackgroundColor;
    int32_t unknown;
    uint32_t flags;
    uint32_t backgroundOffset;
    uint32_t viewOffset;
    uint32_t objectOffset;
    uint32_t tileOffset;
    uint32_t world;
    Rect bounds;
    PointF gravity;
    float metersPerPixel;
} Room;

extern void RoomPrintCSVHeader(FILE *file) {
    fprintf(file, "name,caption,sizeWidth,sizeHeight,speed,isPersistent,color,isDrawBackgroundColor,unknown,flags,world,boundsLeft,boundsRight,boundsBottom,boundsTop,gravityX,gravityY,metersPerPixel\n");
}

extern void RoomPrintCSV(FILE *file, Room r, char *name, char *caption) {
    fprintf(file, "%s,%s,%u,%u,%u,%u,%x,%u,%d,%u,%u,%u,%u,%u,%u,%f,%f,%f\n",
        name, caption,
        r.size.width, r.size.height,
        r.speed, r.isPersistent,
        r.color, r.isDrawBackgroundColor, 
        r.unknown, r.flags, r.world,
        r.bounds.left, r.bounds.right, r.bounds.bottom, r.bounds.top,
        r.gravity.x, r.gravity.y,
        r.metersPerPixel);
}

typedef struct {
    Point position;
    int32_t id;
    int32_t instanceId;
    int32_t createCodeId;
    PointF scale;
    uint32_t color; // argb
    float rotation;
} RoomObject;

extern void RoomObjectPrintCSVHeader(FILE *file) {
    fprintf(file, "room,positionX,positionY,id,instanceId,createCodeId,scaleX,scaleY,color,rotation\n");
}

extern void RoomObjectPrintCSV(FILE *file, RoomObject o, int room) {
    fprintf(file, "%d,%d,%d,%d,%d,%d,%f,%f,%x,%f\n", 
        room,
        o.position.x, o.position.y,
        o.id, o.instanceId, o.createCodeId,
        o.scale.x, o.scale.y,
        o.color, o.rotation);
}

typedef struct {
    float density, restitution, group, linearDamping, angularDamping, unknown1, friction, unknown2, kinematic;
} ObjectPhysics;

typedef struct {
    uint32_t nameOffset;
    int32_t spriteId;
    uint32_t isVisible;
    uint32_t isSolid;
    int32_t depth;
    uint32_t isPersistent;
    int32_t parentId;
    int32_t maskId;
    uint32_t hasPhysics;
    uint32_t isSensor;
    uint32_t collisionShape;
    ObjectPhysics physics;
} GameObject;

extern void GameObjectPrintCSVHeader(FILE *file) {
    fprintf(file, "name,spriteId,isVisible,isSolid,depth,isPersistent,parentId,maskId,hasPhysics,isSensor,collisionShape,density,restitution,group,linearDamping,angularDamping,unknown1,friction,unknown2,kinematic\n");
}

extern void GameObjectPrintCSV(FILE *file, GameObject o, char *name) {
    fprintf(file, "%s,%d,%u,%u,%d,%u,%d,%d,%u,%u,%u,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", 
        name, o.spriteId, 
        o.isVisible, o.isSolid, o.depth, o.isPersistent, 
        o.parentId, o.maskId,
        o.hasPhysics, o.isSensor,
        o.collisionShape, 
        o.physics.density, o.physics.restitution, o.physics.group, 
        o.physics.linearDamping, o.physics.angularDamping, o.physics.unknown1, 
        o.physics.friction, o.physics.unknown2, o.physics.kinematic);
}

typedef struct {
    Point position;
    int32_t id;
    Point sourcePosition;
    Size size;
    uint32_t depth;
    int32_t instanceId;
    PointF scale;
    uint32_t color;
} Tile;

extern void TilePrintCSVHeader(FILE *file) {
    fprintf(file, "room,x,y,id,sourcePositionX,sourcePositionY,width,height,depth,instanceId,scaleX,scaleY,color\n");
}

extern void TilePrintCSV(FILE *file, Tile o, int room) {
    fprintf(file, "%d,%d,%d,%d,%d,%d,%u,%u,%u,%d,%f,%f,%x\n", 
        room,
        o.position.x, o.position.y,
        o.id, 
        o.sourcePosition.x, o.sourcePosition.y,
        o.size.width, o.size.height,
        o.depth, o.instanceId,
        o.scale.x, o.scale.y,
        o.color);
}

typedef struct {
    uint32_t isEnabled;
    Point position;
    Size size;
    Point portPosition;
    Size portSize;
    Point border;
    Point speed;
    int32_t objectId;
} View;

extern void ViewPrintCSVHeader(FILE *file) {
    fprintf(file, "room,isEnabled,positionX,positionY,sizeWidth,sizeHeight,portPositionX,portPositionY,portSizeWidth,portSizeHeight,borderX,borderY,speedX,speedY,objectId\n");
}

extern void ViewPrintCSV(FILE *file, View v, int room) {
    fprintf(file, "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%d,%d,%d\n",
        room,
        v.isEnabled,
        v.position.x,
        v.position.y,
        v.size.width,
        v.size.height,
        v.portPosition.x,
        v.portPosition.y,
        v.portSize.width,
        v.portSize.height,
        v.border.x,
        v.border.y,
        v.speed.x,
        v.speed.y,
        v.objectId);
}

typedef struct {
    uint32_t nameOffset;
    uint32_t size;
} Script;

typedef struct {
    uint32_t nameOffset;
    uint32_t flags;
    uint32_t fileTypeOffset;
    uint32_t fileNameOffset;
    uint32_t unknown;
    float volume;
    float pitch;
    int32_t groupID;
    int32_t audioID;
} Sound;

extern void SoundPrintCSVHeader(FILE *file) {
    fprintf(file, "nameOffset,flags,fileType,fileName,unknown,volume,pitch,groupID,audioID\n");
}

extern void SoundPrintCSV(FILE *file, Sound s, char *name, char *fileType, char *fileName) {
    fprintf(file, "%s,%u,%s,%s,%u,%f,%f,%d,%d\n", name, s.flags, fileType, fileName, s.unknown, s.volume, s.pitch, s.groupID, s.audioID);
}

typedef struct {
    uint32_t nameOffset;
    uint32_t isSmooth;
    uint32_t isClosed;
    uint32_t precision;
} Path;

extern void PathPrintCSVHeader(FILE *file) {
    fprintf(file, "name,isSmooth,isClosed,precision\n");
}

extern void PathPrintCSV(FILE *file, Path p, char *name) {
    fprintf(file, "%s,%u,%u,%u\n", name, p.isSmooth, p.isClosed, p.precision);
}

typedef struct {
    float x, y, speed;
} PathPoint;

extern void PathPointPrintCSVHeader(FILE *file) {
    fprintf(file, "path,x,y,speed\n");
}

extern void PathPointPrintCSV(FILE *file, PathPoint p, int path) {
    fprintf(file, "%d,%f,%f,%f\n", path, p.x, p.y, p.speed);
}

extern void writeRoomXML(FILE *file, Room r, char *caption, 
    Background *backgrounds, char **backgroundNames, int backgroundCount, 
    View *views, char **viewNames, int viewCount, 
    RoomObject *objects, char **objectNames, int objectCount, 
    Tile *tiles, int tileCount) {

    fprintf(file, "<room>\n");
    fprintf(file, "\t<caption>%s</caption>\n", caption);
    fprintf(file, "\t<width>%d</width>\n", r.size.width);
    fprintf(file, "\t<height>%d</height>\n", r.size.height);
    fprintf(file, "\t<vsnap>%d</vsnap>\n", 32);
    fprintf(file, "\t<hsnap>%d</hsnap>\n", 32);
    fprintf(file, "\t<isometric>%d</isometric>\n", 0);
    fprintf(file, "\t<speed>%d</speed>\n", r.speed);
    fprintf(file, "\t<persistent>%d</persistent>\n", XMLBool(r.isPersistent));
    fprintf(file, "\t<colour>%u</colour>\n", r.color);
    fprintf(file, "\t<showcolour>%d</showcolour>\n", XMLBool(r.isDrawBackgroundColor));
    fprintf(file, "\t<code></code>\n");
    fprintf(file, "\t<enableViews>%u</enableViews>\n", 0);
    fprintf(file, "\t<clearViewBackground>%d</clearViewBackground>\n", -1);
    fprintf(file, "\t<clearDisplayBuffer>%d</clearDisplayBuffer>\n", -1);

    // makerSettings
    fprintf(file, "\t<makerSettings>\n");
    fprintf(file, "\t\t<isSet>-1</isSet>\n");
    fprintf(file, "\t\t<w>1283</w>\n");
    fprintf(file, "\t\t<h>798</h>\n");
    fprintf(file, "\t\t<showGrid>0</showGrid>\n");
    fprintf(file, "\t\t<showObjects>-1</showObjects>\n");
    fprintf(file, "\t\t<showTiles>-1</showTiles>\n");
    fprintf(file, "\t\t<showBackgrounds>-1</showBackgrounds>\n");
    fprintf(file, "\t\t<showForegrounds>-1</showForegrounds>\n");
    fprintf(file, "\t\t<showViews>0</showViews>\n");
    fprintf(file, "\t\t<deleteUnderlyingObj>0</deleteUnderlyingObj>\n");
    fprintf(file, "\t\t<deleteUnderlyingTiles>-1</deleteUnderlyingTiles>\n");
    fprintf(file, "\t\t<page>1</page>\n");
    fprintf(file, "\t\t<xoffset>0</xoffset>\n");
    fprintf(file, "\t\t<yoffset>0</yoffset>\n");
    fprintf(file, "\t</makerSettings>\n");

    fprintf(file, "\t<backgrounds>\n");
    for (int i = 0; i < backgroundCount; i++) {
        Background b = backgrounds[i];
        char *name = "";//backgroundNames[i];
        fprintf(file, "\t\t<background visible=\"%d\" foreground=\"%d\" name=\"%s\" x\"%d\" y=\"%d\" htiled=\"%d\" vtiled=\"%d\" hspeed=\"%d\" vspeed=\"%d\" stretch=\"%d\"/>\n",
                XMLBool(b.isEnabled), XMLBool(b.isForeground), name, 
                b.position.x, b.position.y, 
                XMLBool(b.isTileX), XMLBool(b.isTileY),
                b.speed.x, b.speed.y,
                XMLBool(b.isStretch) 
            );
    }
    fprintf(file, "\t</backgrounds>\n");

    fprintf(file, "\t<views>\n");
    for (int i = 0; i < viewCount; i++) {
        View v = views[i];
        char *name = "";//viewNames[i];
        fprintf(file, "\t\t<view visible=\"%d\" objName=\"%s\" xview=\"%d\" yview=\"%d\" wview=\"%d\" hview=\"%d\" xport=\"%d\" yport=\"%d\" wport=\"%d\" hport=\"%d\" hborder=\"%d\" vborder=\"%d\" hspeed=\"%d\" vspeed=\"%d\"/>\n",
                XMLBool(v.isEnabled), name, 
                v.position.x, v.position.y, 
                v.size.width, v.size.height,
                v.portPosition.x, v.portPosition.y,
                v.portSize.width, v.portSize.height,
                v.border.x, v.border.y,
                v.speed.x, v.speed.y
            );
    }
    fprintf(file, "\t</views>\n");

    fprintf(file, "\t<instances>\n");
    for (int i = 0; i < objectCount; i++) {
        RoomObject o = objects[i];
        char name[1000];
        char *objName = "";//objectNames[i];
        sprintf(name, "inst_%x", o.instanceId);
        fprintf(file, "\t\t<instance objName=\"%s\" x=\"%d\" y=\"%d\" name=\"%s\" locked=\"%d\" code=\"\" scaleX=\"%f\" scaleY=\"%f\" colour=\"%u\" rotation=\"%f\"/>\n",
                objName,
                o.position.x, o.position.y, 
                name,
                0,
                o.scale.x,
                o.scale.y,
                o.color,
                o.rotation
            );
    }
    fprintf(file, "\t</instances>\n");

    // TODO: add tile

    fprintf(file, "\t<PhysicsWorld>%d</PhysicsWorld>\n", r.world);
    fprintf(file, "\t<PhysicsWorldTop>%d</PhysicsWorldTop>\n", r.bounds.top);
    fprintf(file, "\t<PhysicsWorldLeft>%d</PhysicsWorldLeft>\n", r.bounds.left);
    fprintf(file, "\t<PhysicsWorldRight>%d</PhysicsWorldRight>\n", r.bounds.right);
    fprintf(file, "\t<PhysicsWorldBottom>%d</PhysicsWorldBottom>\n", r.bounds.bottom);
    fprintf(file, "\t<PhysicsWorldGravityX>%f</PhysicsWorldGravityX>\n", r.gravity.x);
    fprintf(file, "\t<PhysicsWorldGravityY>%f</PhysicsWorldGravityY>\n", r.gravity.y);
    fprintf(file, "\t<PhysicsWorldPixToMeters>%d</PhysicsWorldPixToMeters>\n", r.metersPerPixel);

    fprintf(file, "</room>\n");
}
