#ifndef SCENIC_MAP_H
#define SCENIC_MAP_H

#include "data.h"

/* 高亮路径：一组景点下标序列（含途经点），相邻项之间画高亮线 */
typedef struct {
    int indices[MAX_SPOTS];
    int count;
} MapPath;

/* 渲染整张地图到 stdout。highlight 为 NULL 时不高亮。 */
void map_render(const Graph *g, const MapPath *highlight);

/* 由下标数组构造 MapPath */
void mappath_from_indices(MapPath *mp, const int *idx, int n);

#endif /* SCENIC_MAP_H */
