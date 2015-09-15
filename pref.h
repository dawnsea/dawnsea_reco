#ifndef __pref_h__
#define __pref_h__

int pref_insert(struct rb_root *root, long key, double pref);
int pref_erase(struct rb_root *root, long key);
void pref_erase_all(struct rb_root *root);

#endif
