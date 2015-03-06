#include "Key.h"

Key::Key() {}
Key::Key(float x, float y, SheetSprite sprite) : Entity(x, y, sprite.texWidth, sprite.texHeight, sprite) {}
Key::Key(const Key& rhs) : Entity(rhs) {}