#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PHOTOS 100
#define MAX_ALBUMS 50

// ===== STRUCTURES =====

typedef struct {
    int id;
    char name[50];
    int size;
} Photo;

typedef struct {
    char name[50];
    int photoIds[100];
    int photoCount;
} Album;

// ===== GLOBAL DATA =====

Photo photos[MAX_PHOTOS];
Album albums[MAX_ALBUMS];

int photoCount = 0;
int albumCount = 0;

// ===== UTIL =====

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

int readInt() {
    int x;
    if (scanf("%d", &x) != 1) {
        printf("Invalid input!\n");
        exit(1);
    }
    return x;
}

// ===== HELPERS =====

int photoExists(int id) {
    for (int i = 0; i < photoCount; i++)
        if (photos[i].id == id)
            return 1;
    return 0;
}

// ===== FILE READ =====

void loadPhotos() {
    FILE *f = fopen("photos.txt", "r");
    if (!f) return;

    photoCount = 0;

    while (fscanf(f, "%d %s %d",
                  &photos[photoCount].id,
                  photos[photoCount].name,
                  &photos[photoCount].size) != EOF) {

        if (photoCount >= MAX_PHOTOS) break;
        photoCount++;
    }

    fclose(f);
}

void loadAlbums() {
    FILE *f = fopen("albums.txt", "r");
    if (!f) return;

    albumCount = 0;

    while (fscanf(f, "%s %d",
                  albums[albumCount].name,
                  &albums[albumCount].photoCount) != EOF) {

        if (albumCount >= MAX_ALBUMS) break;

        for (int i = 0; i < albums[albumCount].photoCount; i++) {
            fscanf(f, "%d", &albums[albumCount].photoIds[i]);
        }

        albumCount++;
    }

    fclose(f);
}

// ===== FILE WRITE =====

void saveAlbums() {
    FILE *f = fopen("albums.txt", "w");
    if (!f) return;

    for (int i = 0; i < albumCount; i++) {
        fprintf(f, "%s %d ", albums[i].name, albums[i].photoCount);

        for (int j = 0; j < albums[i].photoCount; j++) {
            fprintf(f, "%d ", albums[i].photoIds[j]);
        }

        fprintf(f, "\n");
    }

    fclose(f);
}

// ===== FUNCTIONALITY =====

void displayPhotos() {
    printf("\n=== PHOTOS ===\n");

    for (int i = 0; i < photoCount; i++) {
        printf("%d. %s (%d KB)\n",
               photos[i].id,
               photos[i].name,
               photos[i].size);
    }
}

void addPhoto() {
    if (photoCount >= MAX_PHOTOS) {
        printf("Photo limit reached!\n");
        return;
    }

    FILE *f = fopen("photos.txt", "a");
    if (!f) return;

    Photo p;

    printf("Enter ID: ");
    p.id = readInt();

    if (photoExists(p.id)) {
        printf("ID already exists!\n");
        fclose(f);
        return;
    }

    printf("Enter name: ");
    scanf("%s", p.name);

    printf("Enter size: ");
    p.size = readInt();

    fprintf(f, "%d %s %d\n", p.id, p.name, p.size);

    fclose(f);

    printf("Photo added!\n");
}

void createAlbum() {
    if (albumCount >= MAX_ALBUMS) {
        printf("Album limit reached!\n");
        return;
    }

    printf("Album name: ");
    scanf("%s", albums[albumCount].name);

    albums[albumCount].photoCount = 0;
    albumCount++;

    saveAlbums();
    printf("Album created!\n");
}

void addPhotoToAlbum() {
    if (albumCount == 0) {
        printf("No albums available!\n");
        return;
    }

    int albumIndex;

    printf("Select album index (0-%d): ", albumCount - 1);
    albumIndex = readInt();

    if (albumIndex < 0 || albumIndex >= albumCount) {
        printf("Invalid album!\n");
        return;
    }

    displayPhotos();

    int photoId;
    printf("Enter photo ID: ");
    photoId = readInt();

    if (!photoExists(photoId)) {
        printf("Photo does not exist!\n");
        return;
    }

    Album *a = &albums[albumIndex];

    if (a->photoCount >= 100) {
        printf("Album is full!\n");
        return;
    }

    a->photoIds[a->photoCount++] = photoId;

    saveAlbums();
    printf("Photo added to album!\n");
}
void viewAlbums() {
    printf("\n=== ALBUMS ===\n");

    for (int i = 0; i < albumCount; i++) {
        printf("%d. %s (%d photos)\n",
               i,
               albums[i].name,
               albums[i].photoCount);
    }
}

void showAlbumSize() {
    int index;

    printf("Album index: ");
    index = readInt();

    if (index < 0 || index >= albumCount) return;

    int total = 0;

    for (int i = 0; i < albums[index].photoCount; i++) {
        int id = albums[index].photoIds[i];

        for (int j = 0; j < photoCount; j++) {
            if (photos[j].id == id) {
                total += photos[j].size;
            }
        }
    }

    printf("Total size: %d KB\n", total);
}

void deleteAlbum() {
    int index;

    printf("Album index to delete: ");
    index = readInt();

    if (index < 0 || index >= albumCount) return;

    for (int i = index; i < albumCount - 1; i++) {
        albums[i] = albums[i + 1];
    }

    albumCount--;

    saveAlbums();
    printf("Album deleted!\n");
}

// ===== MENU =====

void menu() {
    int choice;

    while (1) {
        printf("\n=== PHOTO GALLERY ===\n");
        printf("1. View Photos\n");
        printf("2. Add Photo\n");
        printf("3. Create Album\n");
        printf("4. View Albums\n");
        printf("5. Add Photo to Album\n");
        printf("6. Show Album Size\n");
        printf("7. Delete Album\n");
        printf("0. Exit\n");

        printf("Choice: ");
        choice = readInt();

        clearScreen();

        switch (choice) {
            case 1:
                displayPhotos();
                pauseScreen();
                break;
            case 2:
                addPhoto();
                pauseScreen();
                break;
            case 3:
                createAlbum();
                pauseScreen();
                break;
            case 4:
                viewAlbums();
                pauseScreen();
                break;
            case 5:
                addPhotoToAlbum();
                pauseScreen();
                break;
            case 6:
                showAlbumSize();
                pauseScreen();
                break;
            case 7:
                deleteAlbum();
                pauseScreen();
                break;
            case 0:
                return;
            default:
                printf("Invalid choice!\n");
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