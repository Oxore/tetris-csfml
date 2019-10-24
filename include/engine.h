struct game;
struct idlist;

void transition_init(struct game *game);
void main_loop(struct game *game, const struct idlist *events);
