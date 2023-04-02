void neutralize_virus(char *fileName, int signatureOffset) {
    // Open the file for reading and writing
    FILE *file = fopen(fileName, "r+b");
    if (file == NULL) {
        printf("Error: failed to open file '%s'\n", fileName);
        return;
    }

    // Find the size of the file
    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read the entire file into memory
    char *fileBuffer = (char *) malloc(fileSize);
    fread(fileBuffer, 1, fileSize, file);

    // Scan the file for viruses and neutralize them
    link *currentVirus = virus_list;
    while (currentVirus != NULL) {
        // Check if the virus signature matches at the given offset
        if (memcmp(fileBuffer + signatureOffset, currentVirus->virus, currentVirus->length) == 0) {
            printf("Found virus '%s' at offset %d\n", currentVirus->name, signatureOffset);
            // Neutralize the virus by overwriting the first byte with a RET instruction
            char retInstruction = 0xC3; // RET instruction opcode
            fseek(file, signatureOffset, SEEK_SET);
            fwrite(&retInstruction, 1, 1, file);
        }
        currentVirus = currentVirus->next_virus;
    }

    // Clean up
    free(fileBuffer);
    fclose(file);
}
