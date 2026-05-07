#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PHOTOS 100
#define MAX_ALBUMS 10

// ===== COLORS =====
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

// ===== STRUCTS =====
typedef struct {
    int id;
    char name[100];
    int size;
} Photo;

typedef struct {
    char name[100];
    int photoIds[100];
    int photoCount;
} Album;

// ===== GLOBAL DATA =====
Photo photos[MAX_PHOTOS];
Album albums[MAX_ALBUMS];
int photoCount = 0;
int albumCount = 0;

// ===== VALIDATION INPUT =====
int readInt() {
    int x;
    while (scanf("%d", &x) != 1) {
        printf(RED "Invalid input! Enter a number: " RESET);
        while (getchar() != '\n');
    }
    return x;
}

void readString(char *s, int max) {
    while (1) {
        fgets(s, max, stdin);
        if (s[0] != '\n') break;
    }
    s[strcspn(s, "\n")] = 0;
}

// ===== FILE FUNCTIONS =====
void savePhotos() {
    FILE *f = fopen("photos.dat", "wb");
    if (!f) return;
    fwrite(&photoCount, sizeof(int), 1, f);
    fwrite(photos, sizeof(Photo), photoCount, f);
    fclose(f);
}

void loadPhotos() {
    FILE *f = fopen("photos.dat", "rb");
    if (!f) return;
    fread(&photoCount, sizeof(int), 1, f);
    fread(photos, sizeof(Photo), photoCount, f);
    fclose(f);
}

void saveAlbums() {
    FILE *f = fopen("albums.dat", "wb");
    if (!f) return;
    fwrite(&albumCount, sizeof(int), 1, f);
    fwrite(albums, sizeof(Album), albumCount, f);
    fclose(f);
}

void loadAlbums() {
    FILE *f = fopen("albums.dat", "rb");
    if (!f) return;
    fread(&albumCount, sizeof(int), 1, f);
    fread(albums, sizeof(Album), albumCount, f);
    fclose(f);
}

// ===== PHOTO FUNCTIONS =====
int photoExists(int id) {
    for (int i = 0; i < photoCount; i++)
        if (photos[i].id == id)
            return 1;
    return 0;
}

void addPhoto() {
    if (photoCount >= MAX_PHOTOS) {
        printf(RED "Photo limit reached!\n" RESET);
        return;
    }

    Photo p;

    printf("Enter photo ID (number only): ");
    p.id = readInt();

    if (photoExists(p.id)) {
        printf(RED "ID already exists!\n" RESET);
        return;
    }

    printf("Enter photo name: ");
    getchar();
    readString(p.name, 100);

    printf("Enter photo size (positive number, in KB): ");
    p.size = readInt();
    if (p.size <= 0) {
        printf(RED "Size must be positive!\n" RESET);
        return;
    }

    photos[photoCount++] = p;
    savePhotos();

    printf(GREEN "Photo added successfully!\n" RESET);
}

void displayPhotos() {
    printf("\n=== " CYAN "PHOTOS" RESET " ===\n");

    if (photoCount == 0) {
        printf(RED "No photos found!\n" RESET);
        return;
    }

    for (int i = 0; i < photoCount; i++)
        printf("%d. ID: %d | Name: " GREEN "%s" RESET " | Size: %d KB\n",
               i + 1, photos[i].id, photos[i].name, photos[i].size);
}

// ===== ALBUM FUNCTIONS =====
void createAlbum() {
    if (albumCount >= MAX_ALBUMS) {
        printf(RED "Album limit reached!\n" RESET);
        return;
    }

    Album a;
    a.photoCount = 0;

    printf("Enter album name: ");
    getchar();
    readString(a.name, 100);

    albums[albumCount++] = a;
    saveAlbums();

    printf(GREEN "Album created!\n" RESET);
}

void menu() {
    int choice;

    while (1) {
        printf("\n=== " BLUE "PHOTO GALLERY" RESET " ===\n");
        printf("1. View Photos\n");
        printf("2. Add Photo\n");
        printf("3. Create Album\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");

        choice = readInt();

        switch (choice) {
            case 1: displayPhotos(); break;
            case 2: addPhoto(); break;
            case 3: createAlbum(); break;
            case 0: return;
            default: printf(RED "Invalid choice!\n" RESET);
        }

        printf("\nPress ENTER to continue...");
        getchar();
        getchar();
    }
}

int main() {
    loadPhotos();
    loadAlbums();
    menu();
    return 0;
}