#include <stdio.h>
#include <stdlib.h>

// Структуры для BMP файла
typedef struct {
    short type;
    int size;
    short reserved1;
    short reserved2;
    int offset;
} BMP_Header;

typedef struct {
    int size;
    int width;
    int height;
    short planes;
    short bit_count;
    int compression;
    int image_size;
    int x_pixels;
    int y_pixels;
    int colors_used;
    int colors_important;
} BMP_Info;

// Функция для рисования линии
void draw_line_simple(unsigned char* data, int w, int h, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    
    // Простой алгоритм рисования линии
    if (dx == 0) {
        // Вертикальная линия
        for (int y = y1; y <= y2; y++) {
            if (x1 >= 0 && x1 < w && y >= 0 && y < h) {
                int pos = ((h - 1 - y) * w + x1) * 3;
                data[pos] = b;
                data[pos + 1] = g;
                data[pos + 2] = r;
            }
        }
    } else {
        // Горизонтальная или наклонная линия
        for (int x = x1; x <= x2; x++) {
            int y = y1 + (x - x1) * dy / dx;
            if (x >= 0 && x < w && y >= 0 && y < h) {
                int pos = ((h - 1 - y) * w + x) * 3;
                data[pos] = b;
                data[pos + 1] = g;
                data[pos + 2] = r;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // Открываем файл
    FILE* f = fopen(argv[1], "rb+");
    if (f == NULL) {
        printf("Ошибка открытия файла!\n");
        return 1;
    }
    
    // Читаем заголовки
    BMP_Header header;
    BMP_Info info;
    
    fread(&header, sizeof(header), 1, f);
    fread(&info, sizeof(info), 1, f);
    
    printf("Размер изображения: %d x %d\n", info.width, info.height);
    
    // Вычисляем размер строки с выравниванием
    int row_size = ((info.width * 3 + 3) / 4) * 4;
    
    // Выделяем память для пикселей
    unsigned char* pixels = (unsigned char*)malloc(row_size * info.height);
    if (pixels == NULL) {
        printf("Ошибка памяти!\n");
        fclose(f);
        return 1;
    }
    
    // Переходим к данным пикселей и читаем их
    fseek(f, header.offset, SEEK_SET);
    fread(pixels, 1, row_size * info.height, f);
    
    // Рисуем две диагональные линии
    printf("Рисую линии...\n");
    
    // Первая линия из левого верхнего в правый нижний угол
    draw_line_simple(pixels, info.width, info.height, 0, 0, info.width-1, info.height-1, 255, 0, 0);
    
    // Вторая линия из левого нижнего в правый верхний угол  
    draw_line_simple(pixels, info.width, info.height, 0, info.height-1, info.width-1, 0, 0, 255, 0);
    
    // Сохраняем изменения обратно в файл
    fseek(f, header.offset, SEEK_SET);
    fwrite(pixels, 1, row_size * info.height, f);
    
    // Освобождаем память и закрываем файл
    free(pixels);
    fclose(f);
    
    printf("Готово! Файл изменен.\n");
    return 0;
}