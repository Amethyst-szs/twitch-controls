#pragma once

namespace al
{
    class Projection {
        public:
            float getFovy(void) const;
            void setFovy(float);

            unsigned char padding[0xa0];
            float fovy;
    };
};