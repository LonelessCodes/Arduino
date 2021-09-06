struct AnimationObject {
  unsigned long id;
  struct AnimationObject *next;
  void update() {
    
  }
};

struct {
  AnimationObject *list;
  unsigned long inc = 0;

  void add() {
    struct AnimationObject *item = malloc(sizeof(struct AnimationObject));
    item->id = inc++;
  }

  void update() {
    AnimationObject *curr = list;
    while (curr != nullptr) {
      curr->update();
      curr = curr->next;
    }
  }
} Animation;