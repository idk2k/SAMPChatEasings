#ifndef STRUCTURES_H
#define STRUCTURES_H

namespace structures {
    using D3DCOLOR = DWORD;
    class CRect {
    public:
        long left, top;
        long right, bottom;
    };

}  // namespace structures

#endif  // STRUCTURES_H