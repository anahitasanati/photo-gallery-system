#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PHOTOS 100
#define MAX_ALBUMS 10

// ===== COLORS (ANSI) =====
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
    int size;          // سایز به صورت int (برای جمع‌زدن و جلوگیری از منفی شدن)
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

// ===== UTILS & VALIDATION =====
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

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    printf("\nPress ENTER to continue...");
    while (getchar() != '\n');
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

    // چک تکراری بودن ID
    for (int i = 0; i < photoCount; i++) {
        if (photos[i].id == p.id) {
            printf(RED "ID already exists!\n" RESET);
            return;
        }
    }

    printf("Enter photo name (letters and spaces): ");
    getchar(); // خوردن \n قبلی
    fgets(p.name, 100, stdin);
    p.name[strcspn(p.name, "\n")] = 0;

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

    for (int i = 0; i < photoCount; i++) {
        printf("%d. ID: %d | Name: " GREEN "%s" RESET " | Size: %d KB\n",
               i + 1,
               photos[i].id,
               photos[i].name,
               photos[i].size);
    }
}

// ===== ALBUM FUNCTIONS =====
void viewAlbums() {
    printf("\n=== " CYAN "ALBUMS" RESET " ===\n");

    if (albumCount == 0) {
        printf(RED "No albums found!\n" RESET);
        return;
    }

    for (int i = 0; i < albumCount; i++) {
        printf("%d. Name: " GREEN "%s" RESET " | Photos: %d\n",
               i, albums[i].name, albums[i].photoCount);
    }
}

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

    printf(GREEN "Album created successfully!\n" RESET);
}

void addPhotoToAlbum() {
    viewAlbums();
    if (albumCount == 0) return;

    printf("Enter album index (0 - %d): ", albumCount - 1);
    int albumIndex = readInt();

    if (albumIndex < 0 || albumIndex >= albumCount) {
        printf(RED "Invalid album index!\n" RESET);
        return;
    }

    displayPhotos();
    if (photoCount == 0) return;

    printf("Enter existing photo ID: ");
    int photoId = readInt();

    if (!photoExists(photoId)) {
        printf(RED "Photo does not exist!\n" RESET);
        return;
    }

    Album *a = &albums[albumIndex];

    if (a->photoCount >= 100) {
        printf(RED "Album is full!\n" RESET);
        return;
    }

    a->photoIds[a->photoCount++] = photoId;
    saveAlbums();

    printf(GREEN "Photo added to album successfully!\n" RESET);
}

void showAlbumSize() {
    viewAlbums();
    if (albumCount == 0) return;

    printf("Enter album index (0 - %d): ", albumCount - 1);
    int index = readInt();

    if (index < 0 || index >= albumCount) {
        printf(RED "Invalid album index!\n" RESET);
        return;
    }

    int total = 0;

    for (int i = 0; i < albums[index].photoCount; i++) {
        int id = albums[index].photoIds[i];
        for (int j = 0; j < photoCount; j++)
            if (photos[j].id == id)
                total += photos[j].size;
    }

    printf(GREEN "Total album size: %d KB\n" RESET, total);
}

void deleteAlbum() {
    viewAlbums();
    if (albumCount == 0) return;

    printf("Enter album index to delete (0 - %d): ", albumCount - 1);
    int index = readInt();

    if (index < 0 || index >= albumCount) {
        printf(RED "Invalid album index!\n" RESET);
        return;
    }

    for (int i = index; i < albumCount - 1; i++)
        albums[i] = albums[i + 1];

    albumCount--;
    saveAlbums();

    printf(GREEN "Album deleted successfully!\n" RESET);
}

// ===== MENU =====
void menu() {
    int choice;

    while (1) {
        printf("\n=== " CYAN "PHOTO GALLERY" RESET " ===\n");

        printf("1. View Photos\n");
        printf("2. Add Photo\n");
        printf("3. Create Album\n");
        printf("4. View Albums\n");
        printf("5. Add Photo to Album\n");
        printf("6. Show Album Size\n");
        printf("7. Delete Album\n");
        printf("0. Exit\n");

        printf("Enter your choice (0-7): ");
        choice = readInt();

        clearScreen();

        switch (choice) {
            case 1: displayPhotos();     pauseScreen(); break;
            case 2: addPhoto();          pauseScreen(); break;
            case 3: createAlbum();       pauseScreen(); break;
            case 4: viewAlbums();        pauseScreen(); break;
            case 5: addPhotoToAlbum();   pauseScreen(); break;
            case 6: showAlbumSize();     pauseScreen(); break;
            case 7: deleteAlbum();       pauseScreen(); break;

            case 0:
                printf(GREEN "Exiting program...\n" RESET);
                return;

            default:
                printf(RED "Invalid choice! Please select 0-7.\n" RESET);
                pauseScreen();
        }

        clearScreen();
    }
}

// ===== MAIN =====
int main() {
    loadPhotos();
    loadAlbums();
    clearScreen();
    menu();
    return 0;
}