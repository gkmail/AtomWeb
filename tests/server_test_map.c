/*Generated by aw-scanner*/

#include <atomweb.h>

extern const AW_Class aw_page1_html_class;
extern const AW_Class aw_page2_html_class;
extern const AW_Class aw_sub_page1_html_class;
extern const AW_Class aw_sub_page2_html_class;
extern const AW_Class aw_sub_index_html_class;
extern const AW_Class aw_index_html_class;

static const AW_MapNode aw_map_nodes_0[] = {
	{7, -1, NULL, NULL},
	{1, -1, NULL, NULL},
	{2, -1, NULL, NULL},
	{3, -1, NULL, NULL},
	{5, -1, NULL, NULL},
	{-1, -1, "page1.html", &aw_page1_html_class},
	{-1, -1, "page2.html", &aw_page2_html_class},
	{-1, 1, "sub", NULL},
	{-1, -1, "index.html", &aw_index_html_class}
};
static const AW_MapLink aw_map_links_0[] = {
	{112, -1, 1},
	{97, -1, 2},
	{103, -1, 3},
	{101, -1, 4},
	{49, -1, 5},
	{50, 4, 6},
	{115, 0, 7},
	{105, 6, 8}
};
static const AW_MapNode aw_map_nodes_1[] = {
	{6, -1, NULL, NULL},
	{1, -1, NULL, NULL},
	{2, -1, NULL, NULL},
	{3, -1, NULL, NULL},
	{5, -1, NULL, NULL},
	{-1, -1, "page1.html", &aw_sub_page1_html_class},
	{-1, -1, "page2.html", &aw_sub_page2_html_class},
	{-1, -1, "index.html", &aw_sub_index_html_class}
};
static const AW_MapLink aw_map_links_1[] = {
	{112, -1, 1},
	{97, -1, 2},
	{103, -1, 3},
	{101, -1, 4},
	{49, -1, 5},
	{50, 4, 6},
	{105, 0, 7}
};
const AW_Map aw_map[] = {
	{aw_map_nodes_0, aw_map_links_0},
	{aw_map_nodes_1, aw_map_links_1}
};
