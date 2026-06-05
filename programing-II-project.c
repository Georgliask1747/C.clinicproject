#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MaxThreshold 100 // this is the threshold of the allowed patients/doctors that the program can support
#define MaxNameLength 31 // 30 plus \0
void printMenu();
// feature functions for patients
int patientAdd(char **name, char **surname, char **securityNumber, char **cellphone, int *age, int patientNumbers);
int patientEdit(char **name, char **surname, char **securityNumber, char **cellphone, int *age, int patientNumbers);
int patientDelete(char **name, char **surname, char **securityNumber, char **cellphone, int *age, int *patientNumbers);
int searchPatient(char **name, char **surname, char **securityNumber, char **cellphone, int *age, int patientNumbers);
int patientSort(char **name, char **surname, char **securityNumber, char **cellphone, int *age, int patientNumbers);
// feature functions for doctors
int doctorAdd(char **docName, char **docSurname, char **docCode, int docIndex, int *docSpecialty);
int doctorEdit(char **docName, char **docSurname, char **docCode, int *docSpecialty, int doctorNumbers);
int doctorDelete(char **docName, char **docSurname, char **docCode, int *docSpecialty, int *doctorNumbers);
// helpfull functions for the the patient features
int addName(char **name, int IndividualIndex);
int addSurname(char **surname, int IndividualIndex);
int addSecurityNumber(char **security_number, int patientNumbers);
int addCellphone(char **cellphone, int patientNumbers);
int searchSecurityNumber(char **securityNumber, int patientNumbers, char *CurrentsecurityNumber);
int checkAge(int age);
// helpful functions for the doctor features
int addDoctorCode(char **doctorCode, int docIndex);
int searchDoctorCode(char **doctorCode, int docIndex, char *CurrentDocCode);
int addSpecialties(int docIndex, int *docSpecialty);

enum Specialties // Placed it outside the function so it was more accessible for the whole program
{
    GENERAL,     // 0
    CARDIOLOGY,  // 1
    PEDIATRICS,  // 2
    ORTHOPEDICS, // 3
    NEUROLOGY,   // 4
    OTHER        // 5
};

// struck file initializations

typedef struct Patient
{
    int insurance_id;
    char first_name[MaxNameLength];
    char last_name[MaxNameLength];
    int age;
    long phone;
    struct Patient *prev, *next; /* unused on disk, kept for layout parity */
} Patient;

typedef struct Doctor
{
    int id;
    char first_name[MaxNameLength];
    char last_name[MaxNameLength];
    int specialty;              // ΔΙΟΡΘΩΘΗΚΕ: Από Specialty έγινε int για συμβατότητα με τον GCC
    struct Doctor *prev, *next; /* unused on disk, kept for layout parity */
} Doctor;

struct Appointment
{
    int id;
    int insurance_id;
    int doctor_id;
    const char *datetime;
};

void printMenu()
{
    printf("=== Hospital Management ===\n");
    printf("Patients:\n");
    printf(" 1. Add patient\n");
    printf(" 2. Edit patient\n");
    printf(" 3. Delete patient\n");
    printf(" 4. Search patient by ID\n");
    printf(" 5. List patients (sorted)\n");
    printf("Doctors:\n");
    printf(" 6. Add doctor\n");
    printf(" 7. Edit doctor\n");
    printf(" 8. Delete doctor\n");
    printf(" 9. List doctors sorted by specialty\n");
    printf("Appointments:\n");
    printf("10. Book appointment\n");
    printf("11. Cancel appointment\n");
    printf("12. Reschedule appointment\n");
    printf("13. Display day agenda\n");
    printf(" 0. Save and exit\n");
    printf("Choose: ");
}
int addName(char **name, int IndividualIndex)
{
    fgets(name[IndividualIndex], MaxNameLength, stdin);              // 30 max chars for the name
    name[IndividualIndex][strcspn(name[IndividualIndex], "\n")] = 0; // same newline
    return 0;
}

int addSurname(char **surname, int IndividualIndex)
{
    fgets(surname[IndividualIndex], MaxNameLength, stdin); // 30 max chars for the surname
    surname[IndividualIndex][strcspn(surname[IndividualIndex], "\n")] = 0;
    return 0;
}

int addSecurityNumber(char **security_number, int patientNumbers)
{
    char CurrentsecurityNumber[17]; // 15 characters plus the null terminator and newline, so 17 characters in total
    fgets(CurrentsecurityNumber, 17, stdin);
    CurrentsecurityNumber[strcspn(CurrentsecurityNumber, "\n")] = 0; // Removes newline
    if (strlen(CurrentsecurityNumber) != 15)                         // assuming the security number is always 14 characters long with characters as well
    {
        printf("Invalid security number. Please enter a valid security number.\n");
        return 1;
    }
    if (searchSecurityNumber(security_number, patientNumbers, CurrentsecurityNumber) != -1)
    {
        return 1; // return to main if the security number is not unique
    }
    strcpy(security_number[patientNumbers], CurrentsecurityNumber); // this also makes the security number unable to be changed but oow well
    return 0;
}

int addCellphone(char **cellphone, int patientIndex)
{
    fgets(cellphone[patientIndex], MaxNameLength, stdin); // 10 digits plus the null terminator, so 11 characters in total
    cellphone[patientIndex][strcspn(cellphone[patientIndex], "\n")] = 0;
    while (strlen(cellphone[patientIndex]) != 10) // standard cellphone number length in many countries is 10 digits plus the null terminator, so 11 characters in total
    {
        printf("Invalid cellphone number. Please enter a valid cellphone number.\n");
        fgets(cellphone[patientIndex], MaxNameLength, stdin);
        cellphone[patientIndex][strcspn(cellphone[patientIndex], "\n")] = 0;
    }
    return 0;
}

int addAge(int *age, int patientNumbers)
{
    int inputage;
    if (scanf("%d", &inputage) != 1)
        inputage = -1;
    while (getchar() != '\n')
        ;
    while (checkAge(inputage))
    {
        printf("Invalid age. Please enter a valid age between 0 and 120.\n");
        printf("Give the age of the patient:\n");
        if (scanf("%d", &inputage) != 1)
            inputage = -1;
        while (getchar() != '\n')
            ;
    }
    age[patientNumbers] = inputage;
    return 0;
}

int searchSecurityNumber(char **securityNumber, int patientNumbers, char *CurrentsecurityNumber)
{
    for (int i = 0; i < patientNumbers; i++)
    {
        if (securityNumber[i] != NULL && securityNumber[i][0] != '\0') // this check if the row pointer is null
        {
            if (strcmp(CurrentsecurityNumber, securityNumber[i]) == 0)
            {
                return i; // return i if the security number already exists and give it to the next function in case it needs editing or removing
            }
        }
    }

    return -1; // return -1 if the security number is unique
}

int checkAge(int age)
{
    if (age < 0 || age > 120)
    {
        return 1;
    }

    return 0;
}

// DOCTORS

int doctorDelete(char **docName, char **docSurname, char **docCode, int *docSpecialty, int *doctorNumbers)
{
    while (getchar() != '\n')
        ;
    printf("Give the Doctor's code you want to delete:\n");
    char CurrentDocCode[12];
    fgets(CurrentDocCode, 12, stdin);
    CurrentDocCode[strcspn(CurrentDocCode, "\n")] = 0;

    int foundIndex = searchDoctorCode(docCode, *doctorNumbers, CurrentDocCode);
    if (foundIndex == -1)
    {
        printf("Doctor not found. Deletion aborted.\n");
        return 1;
    }

    printf("Are you sure you want to delete doctor %s %s? (1 for Yes, 0 for No): ", docName[foundIndex], docSurname[foundIndex]);
    int confirm;
    int check = scanf("%d", &confirm);
    while (getchar() != '\n')
        ;

    if (check != 1 || (confirm != 0 && confirm != 1))
    {
        printf("Invalid input. Deletion cancelled.\n");
        return 1;
    }

    if (confirm == 0)
    {
        printf("Deletion cancelled.\n");
        return 1;
    }
    else
    {
        printf("Doctor %s %s deleted successfully!\n", docName[foundIndex], docSurname[foundIndex]);

        // Shift-Left Logic: Μετατοπίζουμε τους επόμενους γιατρούς κατά μία θέση αριστερά
        for (int i = foundIndex; i < (*doctorNumbers) - 1; i++)
        {
            strcpy(docName[i], docName[i + 1]);
            strcpy(docSurname[i], docSurname[i + 1]);
            strcpy(docCode[i], docCode[i + 1]);
            docSpecialty[i] = docSpecialty[i + 1];
        }

        // Μειώνουμε το συνολικό πλήθος των γιατρών
        (*doctorNumbers)--;

        // Καθαρίζουμε την τελευταία θέση που πλέον είναι "διπλότυπη"
        docCode[*doctorNumbers][0] = '\0';
        docName[*doctorNumbers][0] = '\0';
        docSurname[*doctorNumbers][0] = '\0';
        docSpecialty[*doctorNumbers] = 0;

        return 0;
    }
}

int doctorAdd(char **docName, char **docSurname, char **docCode, int docIndex, int *docSpecialty)
{
    printf("Give the name of the doctor:\n");
    addName(docName, docIndex);
    printf("Give the surname of the doctor:\n");
    addSurname(docSurname, docIndex);
    printf("Give the Doctor's code:\n");
    addDoctorCode(docCode, docIndex);
    printf("Give the Doctor's Specialty:\n");
    addSpecialties(docIndex, docSpecialty);
    printf("Doctor %s %s added successfully!\n", docName[docIndex], docSurname[docIndex]);
    return 0;
}

int doctorEdit(char **docName, char **docSurname, char **docCode, int *docSpecialty, int doctorNumbers)
{
    while (getchar() != '\n')
        ;
    printf("Give the Doctor's code you want to edit:\n");
    char CurrentDocCode[12];
    fgets(CurrentDocCode, 12, stdin);
    CurrentDocCode[strcspn(CurrentDocCode, "\n")] = 0;

    int foundIndex = searchDoctorCode(docCode, doctorNumbers, CurrentDocCode);
    if (foundIndex == -1)
    {
        printf("Doctor not found. Please enter a valid code.\n");
        return 1;
    }

    printf("What would you like to edit? 1. Name | 2. Surname | 3. Specialty\n");
    printf("Choose: ");
    int choice;
    int check = scanf("%d", &choice);
    while (getchar() != '\n')
        ;

    if (check != 1)
    {
        printf("Invalid input. Editing aborted.\n");
        return 1;
    }

    if (choice == 1)
    {
        printf("Give the new name of the doctor:\n");
        addName(docName, foundIndex);
        printf("New name is: %s\n", docName[foundIndex]);
    }
    else if (choice == 2)
    {
        printf("Give the new surname of the doctor:\n");
        addSurname(docSurname, foundIndex);
        printf("New surname is: %s\n", docSurname[foundIndex]);
    }
    else if (choice == 3)
    {
        printf("Give the new specialty of the doctor:\n");
        addSpecialties(foundIndex, docSpecialty);
    }
    else
    {
        printf("Invalid choice. Operation aborted.\n");
        return 1;
    }

    printf("Doctor %s %s edited successfully!\n", docName[foundIndex], docSurname[foundIndex]);
    return 0;
}

int addSpecialties(int docIndex, int *docSpecialty)
{
    printf("Select Specialty:\n");
    printf("0: GENERAL, 1: CARDIOLOGY, 2: PEDIATRICS, 3: ORTHOPEDICS, 4: NEUROLOGY, 5: OTHER\n");
    printf("Choose: ");
    int choice;
    while (scanf("%d", &choice) != 1 || choice < 0 || choice > 5)
    {
        printf("Invalid choice. Please enter a number between 0 and 5:\n");
        while (getchar() != '\n')
            ;
    }
    while (getchar() != '\n')
        ;
    docSpecialty[docIndex] = choice;

    return 0;
}

int addDoctorCode(char **doctorCode, int docIndex)
{
    char CurrentDocCode[12]; // 10 characters plus the null terminator and newline, so 11 characters in total
    fgets(CurrentDocCode, 12, stdin);
    CurrentDocCode[strcspn(CurrentDocCode, "\n")] = 0; // Removes newline
    if (strlen(CurrentDocCode) != 10)                  // assuming the security number is always 14 characters long with characters as well
    {
        printf("Invalid doctor's code. Please enter a valid code.\n");
        return 1;
    }
    if (searchDoctorCode(doctorCode, docIndex, CurrentDocCode) != -1)
    {
        return 1; // same idea here
    }
    strcpy(doctorCode[docIndex], CurrentDocCode); // same as the security
    return 0;
}

int searchDoctorCode(char **doctorCode, int docIndex, char *CurrentDocCode)
{
    for (int i = 0; i < docIndex; i++)
    {
        if (doctorCode[i] != NULL && doctorCode[i][0] != '\0') // same rule as the security one
        {
            if (strcmp(CurrentDocCode, doctorCode[i]) == 0)
            {
                return i; // exactly the same idea here
            }
        }
    }

    return -1;
}

// PATIENTS

int patientSort(char **name, char **surname, char **securityNumber, char **cellphone, int *age, int patientNumbers)
{
    if (patientNumbers == 0)
    {
        printf("There are no patient in the system to sort:\n");
        return 1;
    }
    printf("By which criteria would you like the patients to be sorted?\n");
    printf("1. Name\n");
    printf("2. Surname\n");
    printf("3. Security Number\n");
    printf("4. Age\n");
    printf("Choose: ");
    // didn't add sort based on cellphone because like what's the point
    int sortChoice;
    if (scanf("%d", &sortChoice) != 1 || sortChoice < 1 || sortChoice > 4)
    {
        printf("Invalid choice. Sorting aborted.\n");
        while (getchar() != '\n')
            ;
        return 1;
    }
    while (getchar() != '\n')
        ;
    // the temps for everything we are going to switch
    char tempStr[MaxNameLength];
    int tempInt;
    int shouldSwap;
    // I am not sure if this is the best way to do bubble sort in C, haven't really done one since highschool I suppose
    for (int i = 1; i < patientNumbers; i++)
    {
        for (int j = patientNumbers - 1; j >= i; j--)
        {
            shouldSwap = 0; // there is prb a better way than that I suppose
            if (sortChoice == 1)
            {
                if (strcmp(name[j - 1], name[j]) > 0)
                {
                    shouldSwap = 1;
                }
            }
            else if (sortChoice == 2)
            {
                if (strcmp(surname[j - 1], surname[j]) > 0)
                {
                    shouldSwap = 1;
                }
            }
            else if (sortChoice == 3)
            {
                if (strcmp(securityNumber[j - 1], securityNumber[j]) > 0)
                {
                    shouldSwap = 1;
                }
            }
            else if (sortChoice == 4)
            {
                if (age[j - 1] > age[j])
                {
                    shouldSwap = 1;
                }
            }
            if (shouldSwap)
            {
                // essentialy we are interchanging every data for the patients if one needs channging to match the all of the data with the correct patients
                // names
                strcpy(tempStr, name[j - 1]);
                strcpy(name[j - 1], name[j]);
                strcpy(name[j], tempStr);
                // surnames
                strcpy(tempStr, surname[j - 1]);
                strcpy(surname[j - 1], surname[j]);
                strcpy(surname[j], tempStr);
                // securtiynumbers
                strcpy(tempStr, securityNumber[j - 1]);
                strcpy(securityNumber[j - 1], securityNumber[j]);
                strcpy(securityNumber[j], tempStr);
                // cellphone
                strcpy(tempStr, cellphone[j - 1]);
                strcpy(cellphone[j - 1], cellphone[j]);
                strcpy(cellphone[j], tempStr);
                // age
                tempInt = age[j - 1];
                age[j - 1] = age[j];
                age[j] = tempInt;
            }
        }
    }
    printf("\n=== Sorted Patient List ===\n");
    for (int i = 0; i < patientNumbers; i++)
    {
        printf("%d. %s %s | ID: %s | Age: %d | Phone: %s\n",
               i + 1, name[i], surname[i], securityNumber[i], age[i], cellphone[i]);
    }

    return 0;
}

int patientDelete(char **name, char **surname, char **security_number, char **cellphone, int *age, int *patientNumbers) // passing the pointer of patientNymbers in order to decrease it from main too
{
    printf("Give the security number of the patient you want to delete:\n");
    char CurrentsecurityNumber[17];
    fgets(CurrentsecurityNumber, 17, stdin);
    CurrentsecurityNumber[strcspn(CurrentsecurityNumber, "\n")] = 0;
    int foundIndex = searchSecurityNumber(security_number, *patientNumbers, CurrentsecurityNumber);
    while (foundIndex == -1)
    {
        printf("Patient not found. Please enter a valid security number.\n");
        fgets(CurrentsecurityNumber, 17, stdin);
        CurrentsecurityNumber[strcspn(CurrentsecurityNumber, "\n")] = 0;
    }
    printf("Are you sure you want to delete patient %s %s? (1 for Yes, 0 for No): ", name[foundIndex], surname[foundIndex]);
    int confirm;
    int check = scanf("%d", &confirm);
    while (getchar() != '\n')
        ;
    while (check != 1 || (confirm != 0 && confirm != 1))
    {
        printf("Invalid input. Please enter 1 for Yes or 0 for No.\n");
        check = scanf("%d", &confirm);
        while (getchar() != '\n')
            ;
    }
    if (confirm == 0)
    {
        printf("Deletion cancelled.\n");
        return 1; // return to main if the user cancels the deletion
    }
    else
    {
        printf("Patient %s %s deleted successfully!\n", name[foundIndex], surname[foundIndex]);
        for (int i = foundIndex; i < (*patientNumbers) - 1; i++) // the idea here is to shift all the patients and their data that are after the delted patient one step to left in order to the fiull the space of the deleted patient and then decrease the patientNumbers by one to avoid having a duplicate of the last patient in the system, this way we can also avoid having empty spaces in the middle of the system which can cause problems with the search function and more
        {
            strcpy(name[i], name[i + 1]);
            strcpy(surname[i], surname[i + 1]);
            strcpy(security_number[i], security_number[i + 1]);
            strcpy(cellphone[i], cellphone[i + 1]);
            age[i] = age[i + 1];
        }
        (*patientNumbers)--; // this lowers the whole number of patients
        // this just covers the last patient whihc would be dublicated since we would be going over the *patientNumbers and in tern not updating them
        security_number[*patientNumbers][0] = '\0';
        name[*patientNumbers][0] = '\0';
        surname[*patientNumbers][0] = '\0';
        cellphone[*patientNumbers][0] = '\0';
        age[*patientNumbers] = 0;
        return 0;
    }
}

int patientEdit(char **name, char **surname, char **security_number, char **cellphone, int *age, int patientNumbers)
{
    printf("Give the security number of the patient you want to edit:\n");
    char CurrentsecurityNumber[17];
    fgets(CurrentsecurityNumber, 17, stdin);
    CurrentsecurityNumber[strcspn(CurrentsecurityNumber, "\n")] = 0;
    int foundIndex = searchSecurityNumber(security_number, patientNumbers, CurrentsecurityNumber);
    if (foundIndex == -1) // this covers both the cases of security number not existing ands it being wrong format
    {
        printf("Patient not found. Please enter a valid security number.\n");
        return 1;
    }
    printf("What would you like to edit 1. name 2. surname 3. age 4. cellphone number\n");
    int choice;
    int check = scanf("%d", &choice);
    while (getchar() != '\n')
        ;
    if (check != 1)
    {
        printf("Invalid input. Please enter a number.\n");
        return 1;
    }
    if (choice == 1)
    {
        printf("Editing name...\n");
        printf("Give the new name of the patient:\n");
        addName(name, foundIndex);
        printf("New name is: %s\n", name[foundIndex]);
    }
    else if (choice == 2)
    {
        printf("Editing surname...\n");
        addSurname(surname, foundIndex);
        printf("New surname is: %s\n", surname[foundIndex]);
    }
    else if (choice == 3)
    {
        printf("Editing age...\n");
        addAge(age, foundIndex);
        printf("New age is: %d\n", age[foundIndex]);
    }
    else if (choice == 4)
    {
        printf("Editing cellphone number...\n");
        addCellphone(cellphone, foundIndex);
        printf("New cellphone number is: %s\n", cellphone[foundIndex]);
    }
    else
    {
        printf("Invalid choice. Please enter a valid option.\n");
        return 1;
    }
    printf("Patient %s edited successfully!\n", name[foundIndex], surname[foundIndex]);
    return 0;
}

int patientAdd(char **name, char **surname, char **securityNumber, char **cellphone, int *age, int patientNumbers)
{
    // I optimized the code for specific functions since we are going to use them a lot in the editing and adding patients, so I created specific functions for each of the inputs to make the code cleaner and more modular
    printf("Give your patient security number:\n");
    if (addSecurityNumber(securityNumber, patientNumbers))
    {
        printf("Security number already exists. Please enter a unique security number.\n");
        return 1; // returning to main
    }
    printf("Give the name of the patient:\n");
    addName(name, patientNumbers);
    printf("Give the surname of the patient:\n");
    addSurname(surname, patientNumbers);
    printf("Give the age of the patient:\n");
    addAge(age, patientNumbers);
    printf("Give the cellphone number of the patient:\n");
    if (addCellphone(cellphone, patientNumbers))
    {
        return 1; // returning to main
    }
    printf("Patient %s %s added successfully!\n", name[patientNumbers], surname[patientNumbers]);
    return 0;
}

int searchPatient(char **name, char **surname, char **securityNumber, char **cellphone, int *age, int patientNumbers)
{
    printf("Give the security number of the patient you want to search:\n");
    char CurrentsecurityNumber[17];
    fgets(CurrentsecurityNumber, 17, stdin);
    CurrentsecurityNumber[strcspn(CurrentsecurityNumber, "\n")] = 0;
    int foundIndex = searchSecurityNumber(securityNumber, patientNumbers, CurrentsecurityNumber);
    if (foundIndex != -1)
    {
        printf("Patient found: %s %s, Age: %d, Cellphone: %s\n", name[foundIndex], surname[foundIndex], age[foundIndex], cellphone[foundIndex]);
    }
    else
    {
        printf("Patient not found.\n");
        return 1;
    }
    return 0;
}

int main()
{
    int choice;
    int patientNumbers = 0;
    int doctorNumbers = 0;

    // non defined data is for patients and defined data is for the doctors
    char **cellphone = (char **)malloc(MaxThreshold * sizeof(char *));     // this is the column for the cellphone numbers, I am using a double pointer to create a 2D array of characters, since each patient can have a cellphone number with a of 10 digits plus the null terminator, so 11 characters in total
    char **name = (char **)malloc(MaxThreshold * sizeof(char *));          // same logic here with the 30 max characters for the name
    char **surname = (char **)malloc(MaxThreshold * sizeof(char *));       // same
    char **securityNumber = (char **)calloc(MaxThreshold, sizeof(char *)); // I am using calloc to initialize the pointers to NULL for the search function
    int *age = (int *)malloc(MaxThreshold * sizeof(int));
    char **docName = (char **)malloc(MaxThreshold * sizeof(char *));
    char **docSurname = (char **)malloc(MaxThreshold * sizeof(char *));
    char **docCode = (char **)calloc(MaxThreshold, sizeof(char *));
    int *docSpecialty = (int *)malloc(MaxThreshold * sizeof(int));

    if (!name || !surname || !securityNumber || !cellphone || !age || !docName || !docSurname || !docCode || !docSpecialty) // easy way to check malooc all together for mop up
    {
        printf("Column Memory allocation failed!\n");
        return 1;
    }
    for (int i = 0; i < MaxThreshold; i++)
    {
        name[i] = (char *)malloc(MaxNameLength * sizeof(char));
        surname[i] = (char *)malloc(MaxNameLength * sizeof(char));
        securityNumber[i] = (char *)calloc(MaxNameLength, sizeof(char));
        cellphone[i] = (char *)malloc(MaxNameLength * sizeof(char));
        docName[i] = (char *)malloc(MaxNameLength * sizeof(char));
        docSurname[i] = (char *)malloc(MaxNameLength * sizeof(char));
        docCode[i] = (char *)calloc(MaxNameLength, sizeof(char));
        if (!name[i] || !surname[i] || !securityNumber[i] || !cellphone[i] || !docName[i] || !docSurname[i] || !docCode[i]) // the same for the secondatry row mallocs
        {
            printf("Row Memory allocation failed!\n");
            return 1;
        }
    }

    // reading doctor from file when opening the program (this took a little while)
    FILE *fDoctors = fopen("doctors.dat", "rb");
    if (fDoctors != NULL)
    {
        Doctor tempDoc;
        while (doctorNumbers < MaxThreshold && fread(&tempDoc, sizeof(Doctor), 1, fDoctors) == 1)
        {
            strcpy(docName[doctorNumbers], tempDoc.first_name);
            strcpy(docSurname[doctorNumbers], tempDoc.last_name);
            sprintf(docCode[doctorNumbers], "%d", tempDoc.id);
            docSpecialty[doctorNumbers] = tempDoc.specialty;
            doctorNumbers++;
        }
        fclose(fDoctors);
        printf("Successfully loaded %d doctors from file.\n", doctorNumbers);
    }

    // same thing for the patients
    FILE *fPatients = fopen("patients.dat", "rb");
    if (fPatients != NULL)
    {
        Patient tempPatient;
        while (patientNumbers < MaxThreshold && fread(&tempPatient, sizeof(Patient), 1, fPatients) == 1)
        {
            strcpy(name[patientNumbers], tempPatient.first_name);
            strcpy(surname[patientNumbers], tempPatient.last_name);
            sprintf(securityNumber[patientNumbers], "%d", tempPatient.insurance_id);
            age[patientNumbers] = tempPatient.age;
            // this line saves the whole program
            sprintf(cellphone[patientNumbers], "%ld", tempPatient.phone);
            patientNumbers++;
        }
        fclose(fPatients);
        printf("Successfully loaded %d patients from file.\n", patientNumbers);
    }
    do
    {
        printMenu();
        int check = scanf("%d", &choice);
        while (getchar() != '\n')
            ;
        if (check != 1)
        {
            printf("Invalid input. Please enter a number.\n");
            return 1;
        }
        // patient menu
        if (choice >= 1 && choice <= 5)
        {
            printf("=== Patient Menu ===\n");
            if (choice == 1)
            {
                printf("Adding patient...\n");
                if (!patientAdd(name, surname, securityNumber, cellphone, age, patientNumbers))
                {
                    patientNumbers++;
                }
            }
            else if (choice == 2)
            {
                printf("Editing patient...\n");
                patientEdit(name, surname, securityNumber, cellphone, age, patientNumbers);
            }
            else if (choice == 3)
            {
                printf("Deleting patient...\n");
                patientDelete(name, surname, securityNumber, cellphone, age, &patientNumbers); // since we need to update the patientNumbers as well
            }
            else if (choice == 4)
            {
                printf("Searching patient by ID...\n");
                searchPatient(name, surname, securityNumber, cellphone, age, patientNumbers);
            }
            else if (choice == 5)
            {
                printf("Listing patients sorted...\n");
                patientSort(name, surname, securityNumber, cellphone, age, patientNumbers);
            }
        }
        else if (choice >= 6 && choice <= 9)
        {
            printf("=== Doctor Menu ===\n");
            if (choice == 6)
            {
                printf("Adding doctor...\n");
                if (!doctorAdd(docName, docSurname, docCode, doctorNumbers, docSpecialty))
                {
                    doctorNumbers++;
                }
            }
            else if (choice == 7)
            {
                printf("Editing doctor...\n");
                doctorEdit(docName, docSurname, docCode, docSpecialty, doctorNumbers);
            }
            else if (choice == 8)
            {
                printf("Deleting doctor...\n");
                doctorDelete(docName, docSurname, docCode, docSpecialty, &doctorNumbers);
            }
            else if (choice == 9)
            {
            }
        }
        else if (choice >= 10 && choice <= 13)
        {
            printf("=== Appointment Menu ===\n");
            if (choice == 10)
            {
            }
            else if (choice == 11)
            {
            }
            else if (choice == 12)
            {
            }
            else if (choice == 13)
            {
            }
        }
        else if (choice == 0)
        {
            printf("Saving data and exiting...\n");
            // saving doctors to file
            FILE *fDoctors = fopen("doctors.dat", "wb");
            if (fDoctors != NULL)
            {
                for (int i = 0; i < doctorNumbers; i++)
                {
                    Doctor tempDoc;
                    memset(&tempDoc, 0, sizeof(Doctor)); // resseting just in case

                    tempDoc.id = atoi(docCode[i]);
                    strcpy(tempDoc.first_name, docName[i]);
                    strcpy(tempDoc.last_name, docSurname[i]);
                    tempDoc.specialty = docSpecialty[i];

                    fwrite(&tempDoc, sizeof(Doctor), 1, fDoctors);
                }
                fclose(fDoctors);
            }

            // saving patients to file
            FILE *fPatients = fopen("patients.dat", "wb");
            if (fPatients != NULL)
            {
                for (int i = 0; i < patientNumbers; i++)
                {
                    Patient tempPatient;
                    memset(&tempPatient, 0, sizeof(Patient)); // resetting

                    tempPatient.insurance_id = atoi(securityNumber[i]);
                    strcpy(tempPatient.first_name, name[i]);
                    strcpy(tempPatient.last_name, surname[i]);
                    tempPatient.age = age[i];
                    tempPatient.phone = atol(cellphone[i]);

                    fwrite(&tempPatient, sizeof(Patient), 1, fPatients);
                }
                fclose(fPatients);
            }
            printf("Save completed successfully");
        }
        printf("\n");
    } while (choice != 0);
    for (int i = 0; i < MaxThreshold; i++) // free all the mallocs
    {
        free(name[i]);
        free(surname[i]);
        free(securityNumber[i]);
        free(cellphone[i]);
        free(docName[i]);
        free(docSurname[i]);
        free(docCode[i]);
    }
    free(name);
    free(surname);
    free(securityNumber);
    free(cellphone);
    free(age);
    free(docName);
    free(docSurname);
    free(docCode);
    free(docSpecialty);

    return 0;
}