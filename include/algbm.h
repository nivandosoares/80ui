#ifdef __cplusplus
extern "C" {
#endif

BITMAP *load_gbm(char *filename, RGB *pal);
int save_gbm(char *filename, BITMAP *bmp, RGB *pal);

#ifdef __cplusplus
}
#endif