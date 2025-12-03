//
// Created by Ілля on 14.11.2025.
//

#include "operations.h"

#include <cstdio>
#include <ctime>
#define RED  "\x1b[31m"
#define YEL  "\x1b[33m"
#define CYN  "\x1b[36m"
#define RESET "\x1b[0m"

enum menuOption { editTodolist = 1, goTomorrow = 2, goYesterday = 3, goToTheDate = 4, complete = 5, showAll = 6};
enum toDoListOption { addEvent= 1, deleteEvent = 2, changeEventDate = 3, copyEvent = 4, goBack = 5};
SingleDate today;

Task tasks[24];

char line[512];

void ensure_file_exists(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f) {
        fclose(f);
        return;
    }

    f = fopen(filename, "w");
    if (!f) {
        printf("Cannot create file: %s\n", filename);
        return;
    }
    fclose(f);
}

SingleDate getTodaysDate() {

    time_t now = time(NULL);
    tm* tm_info = localtime(&now);

    today.day = tm_info->tm_mday;
    today.month = tm_info->tm_mon + 1;
    today.year = tm_info->tm_year + 1900;

    printf("%02d/%02d/%04d\n", today.day, today.month, today.year);
    return today;
}
Task readLineToTask(char *line)
{
    Task t;
    char buff[512];

    sscanf(line, "%d %d %d %511[^\n]", &t.day, &t.month, &t.year, buff);

    char *marker = strchr(buff, '$');
    if (!marker) {
        t.name[0] = 0;
        t.duration = t.done = t.priority = 0;
        return t;
    }

    int len = marker - buff;
    strncpy(t.name, buff, len);
    t.name[len] = '\0';

    sscanf(marker + 1, "%d %d %d", &t.duration, &t.done, &t.priority);

    return t;
}
void mainMenu()
{
    today = getTodaysDate();

    while (true)
    {
        printf("1 - edit todo list | 2 - go tomorrow | 3 - go yesterday | 4 - go to the date | 5 - mark as completed | 6 - show all\n");
        showToDolist(today);

        int choice;
        scanf("%d", &choice);

        switch (choice)
        {
        case editTodolist:
            {
                toDolist();

            }
            break;

        case goTomorrow: {
                struct tm t = {};
                t.tm_mday = today.day + 1;
                t.tm_mon  = today.month - 1;
                t.tm_year = today.year - 1900;
                t.tm_hour = 12;
                mktime(&t);

                today.day = t.tm_mday;
                today.month = t.tm_mon + 1;
                today.year = t.tm_year + 1900;

        }
            break;

        case goYesterday: {
                struct tm t = {};
                t.tm_mday = today.day - 1;
                t.tm_mon  = today.month - 1;
                t.tm_year = today.year - 1900;
                t.tm_hour = 12;
                mktime(&t);

                today.day = t.tm_mday;
                today.month = t.tm_mon + 1;
                today.year = t.tm_year + 1900;

        }
            break;

        case goToTheDate:
            printf("DD MM YYYY: ");
            scanf("%d %d %d", &today.day, &today.month, &today.year);
            break;

        case complete:
            {
                {
                    {
                        int d, m, y;
                        char text[200];

                        printf("DD MM YYYY");
                        scanf("%d %d %d", &d, &m, &y);

                        printf("Text: ");
                        scanf(" %[^\n]", text);

                        FILE *in = fopen("archive.txt", "r");
                        if (!in) {
                            printf("Cannot open file!\n");
                            return;
                        }

                        FILE *out = fopen("tmp.txt", "w");
                        if (!out) {
                            fclose(in);
                            printf("Cannot open temporary file!\n");
                            return;
                        }

                        char line[512];
                        int found = 0;

                        while (fgets(line, sizeof(line), in)) {
                            Task t = readLineToTask(line);

                            if (t.day == d && t.month == m && t.year == y &&
                                strcmp(t.name, text) == 0)
                            {
                                found = 1;
                                fprintf(out, "%d %d %d %s$%d 1 %d\n",
                                        t.day, t.month, t.year,
                                        t.name, t.duration, t.priority);
                            }
                            else {
                                fputs(line, out);
                            }
                        }

                        fclose(in);
                        fclose(out);

                        if (!found) {
                            printf("Event not found!!!.\n");
                            remove("tmp.txt");
                            return;
                        }

                        remove("archive.txt");
                        rename("tmp.txt", "archive.txt");

                        printf("Event updated successfully!\n");
                    }
                }

            }
            break;
            case showAll:
                {{
                    FILE *f = fopen("archive.txt", "r");
                    if (!f) {
                        perror("fopen");
                        return;
                    }

                    char line[512];

                    while (fgets(line, sizeof(line), f)) {
                        Task t = readLineToTask(line);

                        if (t.name[0] == '\0')
                            continue;

                        printf("%02d.%02d.%04d | %s | dur=%d done=%d prio=%d\n",
                               t.day, t.month, t.year,
                               t.name, t.duration, t.done, t.priority);
                    }

                    fclose(f);
                }


                }
            break;
        }
        printf("%02d/%02d/%04d\n", today.day, today.month, today.year);

    }
}

void delete_from_archive(int d, int m, int y, const char* text)
{


    FILE* in = fopen("archive.txt", "r");
    if (!in) return;

    FILE* out = fopen("tmp.txt", "w");
    if (!out) { fclose(in); return; }

    int fd, fm, fy;
    char line[512];

    while (fgets(line, sizeof(line), in))
    {
        if (sscanf(line, "%d %d %d", &fd, &fm, &fy) != 3)
            continue;

        int match = (fd == d && fm == m && fy == y && strstr(line, text));

        if (!match)
            fputs(line, out);
    }
    printf("Has been deleted\n");
    fclose(in);
    fclose(out);
    remove("archive.txt");
    rename("tmp.txt", "archive.txt");
}




void showToDolist(SingleDate date)
{
    FILE *file = fopen("archive.txt", "r");
    if (!file) {
        printf("file error\n");
        return;
    }

    printf("Today I am going to...\n");
    char line[512];
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0; // убрать \n
        Task task = readLineToTask(line);
        if (task.day == date.day && task.month == date.month && task.year == date.year)
        {
            if (task.done !=1) {
            switch(task.priority)
            {
            case 1:
                printf(RED "|   | - %s \n" RESET, task.name, task.done);
                break;
            case 2:
                printf(YEL "|   | - %s \n" RESET, task.name, task.done);
                break;
            case 3:
                printf(CYN "|   | - %s \n" RESET, task.name, task.done);
                break;
            default:
                printf("|   | - %s \n", task.name, task.done);
            } } else
            {
                printf("| + | - %s \n", task.name, task.done);
            }
        }
    }
    fclose(file);
}
    void toDolist()
{
    {
        int choice;
        printf("1 - add a new event | 2 - delete the event | 3 - edit the event | 4 - copy event | 5 - go to the main menu\n");
        scanf("%d", &choice);
        switch (choice)
        {
        case addEvent:
            {
                Task t;
                FILE* f;
                if (fopen_s(&f, "archive.txt", "a") != 0) {
                    printf("Cannot open file!\n");
                    break;
                }

                printf("What are you going to do?\n");
                getchar(); // убрать лишний \n
                fgets(t.name, EVENTNAME, stdin);
                t.name[strcspn(t.name, "\n")] = 0;

                printf("Enter the date DD MM YYYY\n");
                scanf("%d %d %d", &t.day, &t.month, &t.year);

                printf("If this event is very important, write down 1\n");
                printf("If this event is slightly important, write down 2\n");
                printf("If this event is just for fun, write down 3\n");
                printf("Otherwise write any other number\n");
                scanf("%d", &t.priority);



                fprintf(f, "%d %d %d %s$%d %d %d \n", t.day, t.month, t.year, t.name, t.duration, t.done, t.priority);
                fclose(f);

            }
            break;

        case deleteEvent: {
                int d, m, y;
                char text[200];

                printf("DD MM YYYY\n");
                scanf("%d %d %d", &d, &m, &y);

                printf("Text: ");
                scanf(" %[^\n]", text);

                delete_from_archive(d, m, y, text);
        }
            break;

        case changeEventDate:
            {
                {
                    int d, m, y;
                    char text[200];

                    printf("DD MM YYYY");
                    scanf("%d %d %d", &d, &m, &y);

                    printf("Text: ");
                    scanf(" %[^\n]", text);

                    FILE *in = fopen("archive.txt", "r");
                    if (!in) {
                        printf("Cannot open file!\n");
                        return;
                    }

                    FILE *out = fopen("tmp.txt", "w");
                    if (!out) {
                        fclose(in);
                        printf("Cannot open temporary file!\n");
                        return;
                    }

                    char line[512];
                    int found = 0;

                    while (fgets(line, sizeof(line), in)) {
                        Task t = readLineToTask(line);

                        if (t.day == d && t.month == m && t.year == y &&
                            strcmp(t.name, text) == 0)
                        {
                            found = 1;
                            printf("The task to be copied: %s\n", t.name);

                            printf("Enter new date (DD MM YYYY): ");
                            scanf("%d %d %d", &t.day, &t.month, &t.year);

                            fprintf(out, "%d %d %d %s$%d %d %d\n",
                                    t.day, t.month, t.year,
                                    t.name, t.duration, t.done, t.priority);
                        }
                        else {
                            fputs(line, out);
                        }
                    }

                    fclose(in);
                    fclose(out);

                    if (!found) {
                        printf("Event not found!!!.\n");
                        remove("tmp.txt");
                        return;
                    }

                    remove("archive.txt");
                    rename("tmp.txt", "archive.txt");

                    printf("Event updated successfully!\n");
                }
            } break;
        case copyEvent:
            {
                int d, m, y;
                char text[200];

                printf("Original date (DD MM YYYY): ");
                scanf("%d %d %d", &d, &m, &y);

                printf("Text: ");
                scanf(" %[^\n]", text);

                FILE *in = fopen("archive.txt", "r");
                if (!in) {
                    printf("Cannot open archive.txt\n");
                    break;
                }

                FILE *out = fopen("tmp.txt", "w");
                if (!out) {
                    fclose(in);
                    printf("Cannot open tmp.txt\n");
                    break;
                }

                char line[512];
                int found = 0;
                Task original;

                while (fgets(line, sizeof(line), in)) {
                    Task t = readLineToTask(line);

                    if (!found &&
                        t.day == d && t.month == m && t.year == y &&
                        strcmp(t.name, text) == 0)
                    {
                        found = 1;
                        original = t;
                    }

                    fputs(line, out);
                }

                if (!found) {
                    printf("Event not found!\n");
                    fclose(in);
                    fclose(out);
                    remove("tmp.txt");
                    break;
                }

                printf("New date (DD MM YYYY): ");
                scanf("%d %d %d", &original.day, &original.month, &original.year);

                fprintf(out, "%d %d %d %s$%d %d %d\n",
                        original.day, original.month, original.year,
                        original.name, original.duration, original.done, original.priority);

                fclose(in);
                fclose(out);

                remove("archive.txt");
                rename("tmp.txt", "archive.txt");

                printf("Event duplicated!\n");
            }
            break;
        case goBack:
            {
                mainMenu();

            }
            break;
        }
    }
}