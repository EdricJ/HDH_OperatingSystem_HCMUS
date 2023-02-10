#ifndef FILETABLE_H
#define FILETABLE_H
#include "openfile.h"
#include "sysdep.h"

#define FILE_MAX 10
#define MODE_READWRITE 0

class FileTable {
   private:
    OpenFile** openFile;
    int* fileOpenMode;

   public:
    // Khởi tạo
    FileTable() {
        openFile = new OpenFile*[FILE_MAX];
        fileOpenMode = new int[FILE_MAX];
    }

    int Insert(char* fileName) {
        int freeIndex = -1;
        int fileDescriptor = -1;
        for (int i = 0; i < FILE_MAX; i++) {
            if (openFile[i] == NULL) {
                freeIndex = i;
                break;
            }
        }

        if (freeIndex == -1) {
            return -1;
        }

        fileDescriptor = OpenForReadWrite(fileName, FALSE);

        if (fileDescriptor == -1) return -1;
        openFile[freeIndex] = new OpenFile(fileDescriptor);
        fileOpenMode[freeIndex] = MODE_READWRITE;

        return freeIndex;
    }

    int Remove(int index) {
        if (index < 0 || index >= FILE_MAX) return -1;
        if (openFile[index]) {
            delete openFile[index];
            openFile[index] = NULL;
            return 0;
        }
        return -1;
    }

    int Read(char* buffer, int charCount, int index) {
        if (index >= FILE_MAX) return -1;
        if (openFile[index] == NULL) return -1;
        int result = openFile[index]->Read(buffer, charCount);
        // return -2 nếu không đọc đủ byte
        if (result != charCount) return -2;
        return result;
    }

    int Write(char* buffer, int charCount, int index) {
        if (index >= FILE_MAX) return -1;
        // Kiểm tra trạng thái
        if (openFile[index] == NULL || fileOpenMode[index] != MODE_READWRITE)
            return -1;
        return openFile[index]->Write(buffer, charCount);
    }

    int Seek(int pos, int index) {
        if (index < 0 || index >= FILE_MAX) return -1;
        if (openFile[index] == NULL) return -1;
        // use seek(-1) to move to the end of file
        if (pos == -1) pos = openFile[index]->Length();
        if (pos < 0 || pos > openFile[index]->Length()) return -1;
        return openFile[index]->Seek(pos);
    }

    // Hủy
    ~FileTable() {
        for (int i = 0; i < FILE_MAX; i++) {
            if (openFile[i]) delete openFile[i];
        }
        delete[] openFile;
        delete[] fileOpenMode;
    }
};

#endif