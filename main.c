#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===== COLORS =====
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"

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

// ===== GLOBAL VARIABLES =====
Photo photos[100];
Album albums[100];
int photoCount = 0;
int albumCount = 0;

// ===== UTILITY FUNCTIONS =====
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    printf(YELLOW "\nPress ENTER to continue..." RESET);
    getchar();
}

int readInt() {
    int value;
    while (scanf("%d", &value) != 1) {
        printf(RED "Invalid input! Please enter a valid number: " RESET);
        while (getchar() != '\n');
    }
    getchar();
    return value;
}

// ===== FILE FUNCTIONS =====
void savePhotos() {
    FILE *f = fopen("photos.dat", "wb");
    if (!f) {
        printf(RED "Error saving photos!\n" RESET);
        return;
    }
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
    if (!f) {
        printf(RED "Error saving albums!\n" RESET);
        return;
    }
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

void displayPhotos() {
    printf("\n=== " CYAN "PHOTOS" RESET " ===\n");

    if (photoCount == 0) {
        printf(RED "No photos found!\n" RESET);
        return;
    }

    for (int i = 0; i < photoCount; i++) {
        printf("%d. ID: %d | Name: " GREEN "%s" RESET " | Size: %d KB\n",
               i + 1, photos[i].id, photos[i].name, photos[i].size);
    }
}

void addPhoto() {
    if (photoCount >= 100) {
        printf(RED "Photo storage is full!\n" RESET);
        return;
    }

    Photo p;

    printf("Enter photo ID (number only): ");
    p.id = readInt();

    if (photoExists(p.id)) {
        printf(RED "Photo ID already exists!\n" RESET);
        return;
    }

    printf("Enter photo name (letters and spaces): ");
    fgets(p.name, sizeof(p.name), stdin);
    p.name[strcspn(p.name, "\n")] = '\0';

    if (strlen(p.name) == 0) {
        printf(RED "Photo name cannot be empty!\n" RESET);
        return;
    }

    printf("Enter photo size (positive number, in KB): ");
    p.size = readInt();

    if (p.size <= 0) {
        printf(RED "Size must be a positive number!\n" RESET);
        return;
    }

    photos[photoCount++] = p;
    savePhotos();

    printf(GREEN "Photo added successfully!\n" RESET);
}

// ===== ALBUM FUNCTIONS =====
void createAlbum() {
    if (albumCount >= 100) {
        printf(RED "Album storage is full!\n" RESET);
        return;
    }

    Album a;

    printf("Enter album name (letters and spaces): ");
    fgets(a.name, sizeof(a.name), stdin);
    a.name[strcspn(a.name, "\n")] = '\0';

    if (strlen(a.name) == 0) {
        printf(RED "Album name cannot be empty!\n" RESET);
        return;
    }

    a.photoCount = 0;
    albums[albumCount++] = a;

    saveAlbums();

    printf(GREEN "Album created successfully!\n" RESET);
}

void viewAlbums() {
    printf("\n=== " CYAN "ALBUMS" RESET " ===\n");

    if (albumCount == 0) {
        printf(RED "No albums found!\n" RESET);
        return;
    }

    for (int i = 0; i < albumCount; i++) {
        printf("%d.Name: " GREEN "%s" RESET " | Photos: %d\n",
               i, albums[i].name, albums[i].photoCount);
    }
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
            case 1: displayPhotos(); pauseScreen(); break;
            case 2: addPhoto(); pauseScreen(); break;
            case 3: createAlbum(); pauseScreen(); break;
            case 4: viewAlbums(); pauseScreen(); break;
            case 5: addPhotoToAlbum(); pauseScreen(); break;
            case 6: showAlbumSize(); pauseScreen(); break;
            case 7: deleteAlbum(); pauseScreen(); break;

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
    menu();
    return 0;
}