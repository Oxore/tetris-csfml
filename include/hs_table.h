struct entry {
    char name[CFG_NAME_MAX];
    size_t score;
};

#define HS_TABLE_ATTR_INVISIBLE (1 << 0)
struct hs_table {
    size_t id;
    size_t attr;
    struct entry entries[CFG_HS_TABLE_SIZE];
    size_t nentries;
    size_t fontsize;
    struct vector2i name_cell_size;
    struct vector2i score_cell_size;
    struct vector2i pos;
};

void hs_table_insert(struct hs_table *table, const char *name, size_t score);
int hs_table_load_from_json_file(struct hs_table *table, const char *fname);
int hs_table_save_to_json_file(const struct hs_table *table, const char *fname);
