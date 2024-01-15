#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <ncurses.h>
#include <unistd.h>

using namespace std;



class Video {
private:
    char title[20];
    int likes;
    int views;
    char uploadDate[20];

public:
    Video(const char* title, int likes, int views, const char* uploadDate) {
        strncpy(this->title, title, 20);
        this->likes = likes;
        this->views = views;
        strncpy(this->uploadDate, uploadDate, 20);
    }
    Video() {
        likes = 0;
        views = 0;
        strcpy(title, "");
        strcpy(uploadDate, "");
    }

    void displayInfo() const {
        cout << "Tytuł: " << title << "\nLikes: " << likes
             << "\nWyświetlenia: " << views << "\nData dodania: " << uploadDate << endl;
    }
    const char *getTitle() const {
        return title;
    }

    int getLikes() const {
        return likes;
    }

    int getViews() const {
        return views;
    }

    const char *getUploadDate() const {
        return uploadDate;
    }
    void setLikes(int likes) {
        Video::likes = likes;
    }

    void setViews(int views) {
        Video::views = views;
    }
    void setTitle(const char* newName) {
        strncpy(title, newName, sizeof(title) - 1);
        title[sizeof(title) - 1] = '\0';
    }
    void setUploadDate(const char* newName) {
        strncpy(uploadDate, newName, sizeof(uploadDate) - 1);
        uploadDate[sizeof(uploadDate) - 1] = '\0';
    }
};



class Channel {
private:
    vector<Video> videos;
    char channelName[20];
public:
    void setVideos(const vector<Video> &videos) {
        Channel::videos = videos;
    }

public:
    Channel(const char* name) {
        strncpy(channelName, name, 20);
    }

    const vector<Video>& getVideos() const {
        return videos;
    }

    void addVideo(const Video& video) {
        videos.push_back(video);
    }

    void displayInfo() const {
        cout << "Kanał: " << channelName << "\nLiczba filmów: " << videos.size() << endl;
    }

    const char* getChannelName() const {
        return channelName;
    }

    void setChannelName(const char* newName) {
        strncpy(channelName, newName, sizeof(channelName) - 1);
        channelName[sizeof(channelName) - 1] = '\0';
    }


    Channel() {
    }
    void clearVideos() {
        videos.clear();
    }

};

class ChannelManager {
private:
    vector<Channel> channels;
    vector<Channel> foundChannels;
    vector<Channel> foundChannelsByVid;
    vector<Channel> trash;
    size_t currentChannelIndex=0;
    size_t currentVideoIndex=0;
    size_t currentSearchIndex = 0;
    size_t currentSearchIndexByVid = 0;
    size_t currentTrashIndex = 0;

    const char* password = "tajnehaslo";
    bool checkPassword(const char* inputPassword) const {
        return strcmp(inputPassword, password) == 0;
    }
public:
    const vector<Channel>& getChannels() const {
        return channels;
    }
    ChannelManager(){
    }
    const vector<Channel>& getFoundChannels() const {
        return foundChannels;
    }

    const vector<Channel>& getFoundChannelsByVid() const {
        return foundChannelsByVid;
    }

    void addChannel(const char* name) {
        char modifiedName[20];
        strcpy(modifiedName, "");
        strcpy(modifiedName, name);
        modifiedName[0] = toupper(modifiedName[0]);
        channels.push_back(Channel(modifiedName));
    }

    void addVideoToChannel(const char* channelName, const Video& video);
    void addVideoToTrash(const char* channelName, const Video& video);
    void saveDataToFile(const char* fileName);
    void loadDataFromFile(const char* fileName);
    void setCurrentChannel(int index);
    void deleteCurrentTrash();
    void displayCurrentChannel();
    void nextChannel();
    void nextFound();
    void previousChannel();
    void previousFound();
    void displayAllChannels();
    void sortChannels(char sortBy, char sortOrder);
    void clearAllData();
    void searchChannelsByLetters(const char* lettersToSearch);
    void displaySearchResults(const vector<Channel>& channels);
    void updateChannel();
    void displayNumberOfChannels() const;
    void displaySingleVideoInChannel();
    void nextVideo();
    void previousVideo();
    void displaySingleVideo(const Channel& channel, size_t videoIndex);
    void restoreChannelFromTrash();
    void moveChannelToTrash();
    void displayTrashChannels() const;
    bool modifyDataWithPassword(const char* inputPassword);
    void clearTrashChannels();
    void addChannelToTrash(const char* channelName);
    void nextTrash();
    void previousTrash();
    void displayAllTrash();
    void displaySearchResultsByVid(const vector<Channel>& channels);
    void searchChannelsByVid(int max, int min);
    void nextFoundByVid();
    void previousFoundByVid();
};

bool ChannelManager::modifyDataWithPassword(const char* inputPassword) {
    return checkPassword(inputPassword);
}

void ChannelManager::displayTrashChannels() const {

    if (!trash.empty()) {
        const Channel& currentTrash = trash[currentTrashIndex];
        currentTrash.displayInfo();
    } else {
        cout << "Kosz jest pusty." << endl;
    }
}

void ChannelManager::clearTrashChannels(){
    for (Channel& channel : trash) {
        channel.clearVideos();
    }
    trash.clear();
    currentTrashIndex = 0;
}

void ChannelManager::moveChannelToTrash() {
    if (currentChannelIndex < channels.size()) {
        trash.push_back(channels[currentChannelIndex]);
        channels.erase(channels.begin() + currentChannelIndex);
        cout << "Przeniesiono kanał do kosza." << endl;

        if (currentChannelIndex >= channels.size()) {
            currentChannelIndex = channels.size() - 1;
        }
    } else {
        cout << "Błąd: Nieprawidłowy indeks bieżącego kanału." << endl;
    }
}

void ChannelManager::restoreChannelFromTrash() {
    if (!trash.empty()) {
        if (currentTrashIndex < trash.size()) {
            const char* channelNameToRestore = trash[currentTrashIndex].getChannelName();

            cout << "Czy na pewno chcesz przywrócić kanał '" << channelNameToRestore << "' z kosza? (t/n): ";
            char confirmation;
            cin >> confirmation;

            if (confirmation == 'T' || confirmation == 't') {
                channels.push_back(trash[currentTrashIndex]);
                trash.erase(trash.begin() + currentTrashIndex);
                cout << "Przywrócono kanał z kosza." << endl;
            } else {
                cout << "Operacja przywracania anulowana." << endl;
            }
        } else {
            cout << "Nieprawidłowy indeks kanału w koszu." << endl;
        }
    } else {
        cout << "Kosz jest pusty." << endl;
    }
}



void ChannelManager::displaySingleVideo(const Channel& channel, size_t videoIndex) {
    const vector<Video>& videosInChannel = channel.getVideos();

    if (videoIndex >= 0 && videoIndex < videosInChannel.size())
    {
        const Video& currentVideo = videosInChannel[videoIndex];
        currentVideo.displayInfo();
    }
    else
    {
        cout << "Błąd: Nieprawidłowy indeks filmu." << endl;
    }
}

void ChannelManager::displaySingleVideoInChannel() {
    if (!channels.empty()) {
        const Channel& currentChannel = channels[currentChannelIndex];
        displaySingleVideo(currentChannel, currentVideoIndex);
    } else {
        cout << "Brak kanałów do wyświetlenia." << endl;
    }
}

void ChannelManager::nextVideo() {
    if (!channels.empty()) {
        const Channel& currentChannel = channels[currentChannelIndex];
        const vector<Video>& videosInChannel = currentChannel.getVideos();

        if (currentVideoIndex < videosInChannel.size() - 1) {
            currentVideoIndex++;
            cout << "Przełączono na nastepny film." << endl;
        } else {
            cout << "Jesteś już na ostatnim filmie w tym kanale." << endl;
        }
    } else {
        cout << "Brak kanałów do przeglądania." << endl;
    }
}

void ChannelManager::previousVideo() {
    if (!channels.empty()) {
        const Channel& currentChannel = channels[currentChannelIndex];
        const vector<Video>& videosInChannel = currentChannel.getVideos();

        if (currentVideoIndex > 0) {
            currentVideoIndex--;
            cout << "Przełączono na poprzedni film." << endl;
        } else {
            cout << "Jesteś już na pierwszym filmie w tym kanale." << endl;
        }
    } else {
        cout << "Brak kanałów do przeglądania." << endl;
    }
}

void ChannelManager::displayNumberOfChannels() const {
    cout<< channels.size()-1;
}

void ChannelManager::updateChannel() {
    if (!channels.empty()) {
        Channel& currentChannel = channels[currentChannelIndex];
        const char* currentChannelName = currentChannel.getChannelName();
        char newChannelName[20];
        cout << "Podaj nową nazwę kanału [" << currentChannelName << "]: ";
        cin.ignore();
        cin.getline(newChannelName, sizeof(newChannelName));
        currentChannel.setChannelName(newChannelName);

        cout << "Aktualizacja nazwy kanału zakończona pomyślnie." << endl;
    } else {
        cout << "Brak kanałów do aktualizacji." << endl;
    }
}


void ChannelManager::displaySearchResults(const vector<Channel>& channels)
{
    if (channels.empty()) {
        cout << "Brak wyników wyszukiwania." << endl;
        return;
    }
    channels[currentSearchIndex].displayInfo();
}

void ChannelManager::displaySearchResultsByVid(const vector<Channel>& channels)
{
    if (channels.empty()) {
        cout << "Brak wyników wyszukiwania." << endl;
        return;
    }
    channels[currentSearchIndexByVid].displayInfo();
}
void ChannelManager::searchChannelsByLetters(const char* lettersToSearch)
{
    foundChannels.clear();

    for (const Channel& channel : channels) {
        if (strstr(channel.getChannelName(), lettersToSearch) != nullptr) {
            foundChannels.push_back(channel);
        }
    }
}

void ChannelManager::searchChannelsByVid(int max, int min)
{
    foundChannelsByVid.clear();
    int ma=max;
    int mi=min;
    for (const Channel& channel : channels) {
        if (channel.getVideos().size() >= mi && channel.getVideos().size() <= ma) {
            foundChannelsByVid.push_back(channel);
        }
    }
}



void ChannelManager::clearAllData() {
    for (Channel& channel : channels) {
        channel.clearVideos();
    }
    channels.clear();
    for (Channel& tra : trash) {
        tra.clearVideos();
    }
    trash.clear();
    foundChannels.clear();
    foundChannelsByVid.clear();
    currentChannelIndex = 0;
    currentVideoIndex=0;
    currentSearchIndex = 0;
    currentSearchIndexByVid = 0;
    currentTrashIndex=0;
}

void ChannelManager::addVideoToChannel(const char* channelName, const Video& video) {
    for (Channel& channel : channels) {
        if (strcmp(channel.getChannelName(), channelName) == 0) {
            channel.addVideo(video);
            cout << "Dodano nowy film do kanalu: " << channelName << endl;
            return;
        }
    }
    cout << "Blad: Nie znaleziono kanalu o nazwie " << channelName << endl;
}

void ChannelManager::addVideoToTrash(const char* channelName, const Video& video) {
    for (Channel& channel : trash) {
        if (strcmp(channel.getChannelName(), channelName) == 0) {
            channel.addVideo(video);
            cout << "Dodano nowy film do kanalu: " << channelName << endl;
            return;
        }
    }
    cout << "Blad: Nie znaleziono kanalu o nazwie " << channelName << endl;
}

void ChannelManager::deleteCurrentTrash() {
    if (currentTrashIndex < trash.size()) {
        trash.erase(trash.begin() + currentTrashIndex);
        cout << "Usunięto bieżący kanał." << endl;

        if (currentTrashIndex >= trash.size()) {
            currentTrashIndex = trash.size() - 1;
        }
    } else {
        cout << "Błąd: Nieprawidłowy indeks bieżącego kanału." << endl;
    }
}


void ChannelManager::displayCurrentChannel() {
    if (!channels.empty()) {
        const Channel& currentChannel = channels[currentChannelIndex];
        currentChannel.displayInfo();
    } else {
        cout << "Brak kanałów do wyświetlenia." << endl;
    }
}

void ChannelManager::nextChannel() {
    if (!channels.empty()) {
        if (currentChannelIndex < channels.size() - 1) {
            currentChannelIndex++;
            cout << "Przełączono na następny kanał." << endl;
        } else {
            cout << "Jesteś już na ostatnim kanale." << endl;
        }
    } else {
        cout << "Brak kanałów do przełączenia." << endl;
    }
}

void ChannelManager::nextFound() {
    if (!foundChannels.empty()) {
        if (currentSearchIndex < foundChannels.size() - 1) {
            currentSearchIndex++;
            cout << "Przełączono na następny kanał." << endl;
        } else {
            cout << "Jesteś już na ostatnim kanale." << endl;
        }
    } else {
        cout << "Brak kanałów do przełączenia." << endl;
    }
}
void ChannelManager::nextFoundByVid() {
    if (!foundChannelsByVid.empty()) {
        if (currentSearchIndexByVid < foundChannelsByVid.size() - 1) {
            currentSearchIndexByVid++;
            cout << "Przełączono na następny kanał." << endl;
        } else {
            cout << "Jesteś już na ostatnim kanale." << endl;
        }
    } else {
        cout << "Brak kanałów do przełączenia." << endl;
    }
}

void ChannelManager::previousChannel() {
    if (!channels.empty()) {
        if (currentChannelIndex > 0) {
            currentChannelIndex--;
            cout << "Przełączono na poprzedni kanał." << endl;
        } else {
            cout << "Jesteś już na pierwszym kanale." << endl;
        }
    } else {
        cout << "Brak kanałów do przełączenia." << endl;
    }
}
void ChannelManager::previousFound() {
    if (!foundChannels.empty()) {
        if (currentSearchIndex > 0) {
            currentSearchIndex--;
            cout << "Przełączono na poprzedni kanał." << endl;
        } else {
            cout << "Jesteś już na pierwszym kanale." << endl;
        }
    } else {
        cout << "Brak kanałów do przełączenia." << endl;
    }
}
void ChannelManager::previousFoundByVid() {
    if (!foundChannelsByVid.empty()) {
        if (currentSearchIndexByVid > 0) {
            currentSearchIndexByVid--;
            cout << "Przełączono na poprzedni kanał." << endl;
        } else {
            cout << "Jesteś już na pierwszym kanale." << endl;
        }
    } else {
        cout << "Brak kanałów do przełączenia." << endl;
    }
}

void ChannelManager::setCurrentChannel(int index) {
    if (index >= 0 && static_cast<size_t>(index) < channels.size()) {
        currentChannelIndex = index;
        cout << "Ustawiono bieżący kanał." << endl;
    } else {
        cout << "Błąd: Nieprawidłowy indeks kanału." << endl;
    }
}

void ChannelManager::displayAllChannels() {
    for (const Channel& channel : channels)
    {
        channel.displayInfo();
        cout << "------------------------" << endl;
    }
    if(channels.empty())
    {
        cout<<"Nie ma zadnych kanalow"<<endl;
    }
}

void ChannelManager::displayAllTrash() {
    for (const Channel& channel : trash)
    {
        channel.displayInfo();
        cout << "------------------------" << endl;
    }
    if(trash.empty())
    {
        cout<<"Nie ma zadnych kanalow"<<endl;
    }
}

void ChannelManager::sortChannels(char sortBy, char sortOrder) {
    vector<Channel>& channels = const_cast<vector<Channel>&>(getChannels());

    switch (sortBy) {
        case 'n':
            sort(channels.begin(), channels.end(), [sortOrder](const Channel& a, const Channel& b) {
                if (sortOrder == 'a') {
                    return strcmp(a.getChannelName(), b.getChannelName()) < 0;
                } else if (sortOrder == 'o') {
                    return strcmp(a.getChannelName(), b.getChannelName()) > 0;
                } else {
                    return false;
                }
            });
            break;

        case 'i':
            sort(channels.begin(), channels.end(), [sortOrder](const Channel& a, const Channel& b) {
                if (sortOrder == 'r') {
                    return a.getVideos().size() < b.getVideos().size();
                } else if (sortOrder == 'm') {
                    return a.getVideos().size() > b.getVideos().size();
                } else {
                    return false;
                }
            });
            break;

        default:
            cout << "Błąd: Nieprawidłowy sposób sortowania." << endl;
            break;
    }
}

void ChannelManager::saveDataToFile(const char* fileName) {
    ofstream file(fileName);

    if (!file) {
        cout << "Błąd otwarcia pliku." << endl;
        return;
    }
    int numChannels = channels.size();
    file << numChannels <<endl;

    for (const Channel& channel : channels) {
        file << channel.getChannelName() << endl;

        const vector<Video>& videosInChannel = channel.getVideos();
        file << videosInChannel.size() << endl;

        for (const Video& video : videosInChannel) {
            file << video.getTitle() << endl;
            file << video.getLikes() << endl;
            file << video.getViews() << endl;
            file << video.getUploadDate() << endl;
        }
    }
    int numChannelsInTrash = trash.size();
    file << numChannelsInTrash << endl;

    for (const Channel& channelTra : trash) {
        file << channelTra.getChannelName() << endl;

        const vector<Video>& videosInTrash = channelTra.getVideos();
        file << videosInTrash.size() << endl;

        for (const Video& video : videosInTrash) {
            file << video.getTitle() << endl;
            file << video.getLikes() << endl;
            file << video.getViews() << endl;
            file << video.getUploadDate() << endl;
        }
    }

    file.close();
}

void ChannelManager::loadDataFromFile(const char* fileName) {
    ifstream file(fileName);

    if (!file) {
        cerr << "Błąd otwarcia pliku." << endl;
        return;
    }


    int numChannels;
    if (!(file >> numChannels)) {
        cerr << "Błąd podczas wczytywania liczby kanałów." << endl;
        return;
    }

    for (int i = 0; i < numChannels; ++i) {
        char channelName[20];
        if (!(file >> channelName)) {
            cerr << "Błąd podczas wczytywania nazwy kanału." << endl;
            return;
        }

        addChannel(channelName);

        int numVideos;
        if (!(file >> numVideos)) {
            cerr << "Błąd podczas wczytywania liczby filmów w kanale." << endl;
            return;
        }

        for (int j = 0; j < numVideos; ++j) {
            char videoTitle[20];
            if (!(file >> videoTitle)) {
                cerr << "Błąd podczas wczytywania tytułu filmu." << endl;
                return;
            }

            int likes, views;
            char uploadDate[20];

            if (!(file >> likes >> views >> uploadDate)) {
                cerr << "Błąd podczas wczytywania danych filmu." << endl;
                return;
            }

            Video newVideo(videoTitle, likes, views, uploadDate);
            addVideoToChannel(channelName, newVideo);
        }
    }

    int numChannelsInTrash;

    if (!(file >> numChannelsInTrash)) {
        cerr << "Nie wczytano liczby kanałów w koszu." << endl;
        return;
    }

    for (int i = 0; i < numChannelsInTrash; ++i) {
        char channelNameTra[20];
        if (!(file >> channelNameTra)) {
            cerr << "Błąd podczas wczytywania nazwy kanału w koszu." << endl;
            return;
        }

        addChannelToTrash(channelNameTra);

        int numVideos;
        if (!(file >> numVideos)) {
            cerr << "Błąd podczas wczytywania liczby filmów w kanale w koszu." << endl;
            return;
        }

        for (int j = 0; j < numVideos; ++j) {
            char trashVideoTitle[20];
            if (!(file >> trashVideoTitle)) {
                cerr << "Błąd podczas wczytywania tytułu filmu w koszu." << endl;
                return;
            }

            int likesTra, viewsTra;
            char uploadDateTra[20];

            if (!(file >> likesTra >> viewsTra >> uploadDateTra)) {
                cerr << "Błąd podczas wczytywania danych filmu w koszu." << endl;
                return;
            }

            Video newVideoTra(trashVideoTitle, likesTra, viewsTra, uploadDateTra);
            addVideoToTrash(channelNameTra, newVideoTra);
        }
    }

    cout << "Dane zostały wczytane z pliku." << endl;
    file.close();
}

void ChannelManager::addChannelToTrash(const char* channelName) {
    char modifiedName[20];
    strcpy(modifiedName, "");
    strcpy(modifiedName, channelName);
    modifiedName[0] = toupper(modifiedName[0]);
    trash.push_back(Channel(modifiedName));
}

void ChannelManager::nextTrash() {
    if (!trash.empty()) {
        if (currentTrashIndex < trash.size() - 1) {
            currentTrashIndex++;
            cout << "Przełączono na następny kanał." << endl;
        } else {
            cout << "Jesteś już na ostatnim kanale." << endl;
        }
    } else {
        cout << "Brak kanałów do przełączenia." << endl;
    }
}

void ChannelManager::previousTrash() {
    if (!trash.empty()) {
        if (currentTrashIndex > 0) {
            currentTrashIndex--;
            cout << "Przełączono na poprzedni kanał." << endl;
        } else {
            cout << "Jesteś już na pierwszym kanale." << endl;
        }
    } else {
        cout << "Brak kanałów do przełączenia." << endl;
    }
}


int main() {
    ChannelManager channelManager;
    vector<Channel> allChannels;
    char channelName[20],videoDate[20],videoTitle[20],zn, fileName[15],lettersToSearch[20],choice, inputPassword[20];
    int videoLikes, videoViews, sortBy, sortOrder, min, max;
    bool passwordCorrect=false;
    int maxAttempts = 3;
    int attempt = 0;
    do{
        cout << "Podaj haslo dostepu: ";
        cin>>inputPassword;
        passwordCorrect = channelManager.modifyDataWithPassword(inputPassword);

        if (passwordCorrect) {
            system("clear");
            cout << "Witamy :)" << endl;
        } else {
            system("clear");
            ++attempt;
            cout << "Nieprawidlowe haslo dostepu. Sprobuj ponownie." << endl;
        }

        if (attempt >= maxAttempts) {
            cout << "\rPrzekroczono limit prob. Czekaj 30 sekund przed kolejna proba. Pozostalo 30 sekund.";
            cout.flush();

            time_t startTime = time(nullptr);
            while (difftime(time(nullptr), startTime) < 30) {
                cout << "\rPrzekroczono limit prob. Czekaj 30 sekund przed kolejna proba. Pozostalo "
                     << 30 - static_cast<int>(difftime(time(nullptr), startTime)) << " sekund. ";
                cout.flush();
                sleep(1);
            }
            cout<<"\n";
            attempt = 0;
        }
    } while (!passwordCorrect);


    do {
        fflush(stdin);
        cout << "\nWybierz opcję:\n"
             << "1. Dodaj nowy kanał\n"
             << "2. Dodaj nowy film do kanału\n"
             << "3. Wyświetl wszystkie kanały\n"
             << "4. Zapisz do pliku\n"
             << "5. Wczytaj z pliku\n"
             << "6. Wyswietl biezacy kanal\n"
             << "8. Sortuj kanaly\n"
             << "9. Wyszukaj kanaly\n"
             << "q. wyczyszcz program\n"
             << "k. Wyswietl zawartosc kosza\n"
             << "e. Wyjście\n";

        cin>>zn;
        system("clear");
        switch (zn) {
            case '1': {

                cout << "Podaj nazwę nowego kanału: ";
                cin>>channelName;
                channelManager.addChannel(channelName);
                break;
            }

            case '2': {
                cout << "Podaj nazwę kanału, do którego chcesz dodać film: ";
                cin >> channelName;
                int day, month, year;
                time_t currentTime = time(nullptr);
                tm* localTime = localtime(&currentTime);
                int currentYear = localTime->tm_year + 1900;
                int currentMonth = localTime->tm_mon + 1;
                int currentDay = localTime->tm_mday;

                do {
                    cout << "Podaj tytuł filmu: ";
                    cin >> videoTitle;
                    if (strlen(videoTitle) == 0) {
                        cout << "Błąd: Tytuł filmu nie może być pusty." << endl;
                    }
                } while (strlen(videoTitle) == 0);

                do {
                    cout << "Podaj ilość polubień filmu: ";
                    cin >> videoLikes;
                    if (videoLikes < 0) {
                        cout << "Błąd: Ilość polubień nie może być ujemna." << endl;
                    }
                } while (videoLikes < 0);

                do {
                    cout << "Podaj ilość wyświetleń filmu: ";
                    cin >> videoViews;
                    if (videoViews < 0) {
                        cout << "Błąd: Ilość wyświetleń nie może być ujemna." << endl;
                    }
                } while (videoViews < 0);

                do {
                    cout << "Podaj datę dodania filmu(DD-MM-RRRR): ";
                    cin >> videoDate;
                    if (strlen(videoDate) != 10 || videoDate[2] != '-' || videoDate[5] != '-') {
                        cout << "Błąd: Nieprawidłowy format daty. Poprawny format to DD-MM-RRRR." << endl;
                        continue;
                    }

                    if (sscanf(videoDate, "%d-%d-%d", &day, &month, &year) != 3) {
                        cout << "Błąd: Nieprawidłowy format daty. Poprawny format to DD-MM-RRRR." << endl;
                        continue;
                    }

                    if (day < 1 || day > 31 || month < 1 || month > 12|| year<2006|| year > currentYear ||
                        (year == currentYear && (month > currentMonth || (month == currentMonth && day > currentDay)))) {
                        cout << "Błąd: Nieprawidłowa data. Dzień (1-31), Miesiąc (1-12), a rok (2006-" << currentYear<<") i nie moze byc z przyszlosci"<<endl;
                        continue;
                    }
                } while (strlen(videoDate) != 10 || videoDate[2] != '-' || videoDate[5] != '-'||
                         day < 1 || day > 31 || month < 1 || month > 12|| year<2006|| year > currentYear ||
                         (year == currentYear && (month > currentMonth || (month == currentMonth && day > currentDay))));

                Video newVideo(videoTitle, videoLikes, videoViews, videoDate);
                channelManager.addVideoToChannel(channelName, newVideo);
                break;
            }



            case '3':
            {
                channelManager.displayAllChannels();
                break;
            }
            case'4':
            {
                do {
                    cout << "Podaj nazwę pliku (bez rozszerzenia): ";
                    cin >> fileName;

                    if (strlen(fileName) == 0) {
                        cout << "Błąd: Nazwa pliku nie może być pusta. Wprowadź nazwę ponownie." << endl;
                        continue;  // Powtórz pętlę
                    } else {
                        strcat(fileName, ".dat");

                        char confirm;
                        cout << "Czy na pewno chcesz zapisać w pliku " << fileName << "? (T/N): ";
                        cin >> confirm;

                        if (confirm == 'T' || confirm == 't') {
                            channelManager.saveDataToFile(fileName);
                            cout << "Dane zostały zapisane w pliku "<<fileName << endl;
                        } else {
                            cout << "Zapisanie danych zostało anulowane." << endl;
                        }
                    }

                    break;
                } while (true);
                break;
            }
            case '5':
            {
                do {
                    cout << "Podaj nazwę pliku (bez rozszerzenia): ";
                    cin >> fileName;

                    if (strlen(fileName) == 0) {
                        cout << "Błąd: Nazwa pliku nie może być pusta. Wprowadź nazwę ponownie." << endl;
                        continue;  // Powtórz pętlę
                    } else {
                        strcat(fileName, ".dat");

                        char confirm;
                        cout << "Czy na pewno chcesz wczytac dane z pliku " << fileName << "? (T/N): ";
                        cin >> confirm;

                        if (confirm == 'T' || confirm == 't') {
                            channelManager.loadDataFromFile(fileName);
                        } else {
                            cout << "Wczytywanie danych zostało anulowane." << endl;
                        }
                    }

                    break;
                } while (true);
                break;
            }
            case '6':
            {
                do
                {
                    cout<<"\n";
                    channelManager.displayCurrentChannel();
                    cout <<endl<< " Następny (n)\n Poprzedni (p)\n Usuń kanał (u)\n Ustaw bieżący (s)\n Zmien nazwę kanału (a)\n wyświetl filmy (v)\n Wróć do strony głównej (w) : ";
                    cin >> choice;
                    switch (choice) {
                        case 'n':
                        {
                            system("clear");
                            channelManager.nextChannel();
                            fflush(stdin);
                            break;
                        }

                        case 'p':
                        {
                            system("clear");
                            channelManager.previousChannel();
                            fflush(stdin);
                            break;
                        }
                        case 'u':
                        {
                            system("clear");
                            channelManager.displayCurrentChannel();
                            cout << "Czy na pewno chcesz usunąć bieżący kanał? (T/N): ";
                            char confirm;
                            cin >> confirm;
                            if (confirm == 'T' || confirm == 't') {
                                channelManager.moveChannelToTrash();
                                fflush(stdin);
                            } else {
                                cout << "Nie usunięto kanału." << endl;
                            }
                            break;
                        }
                        case 'v': {
                            char videoChoice;
                            do {
                                cout<<endl;
                                channelManager.displaySingleVideoInChannel();
                                cout << "Następny film (n)\n Poprzedni film (p)\n Wróć (w): ";
                                cin >> videoChoice;

                                switch (videoChoice) {
                                    case 'n':
                                        system("clear");
                                        channelManager.nextVideo();
                                        fflush(stdin);
                                        break;
                                    case 'p':
                                        system("clear");
                                        channelManager.previousVideo();
                                        fflush(stdin);
                                        break;
                                    case 'w':
                                        fflush(stdin);
                                        break;
                                    default:
                                        cout << "Nieprawidłowy wybór. Spróbuj ponownie." << endl;
                                }
                            } while (videoChoice != 'w');
                            system("clear");
                            break;
                        }

                        case's':
                        {
                            system("clear");
                            cout << "Podaj indeks kanału(0-";
                            channelManager.displayNumberOfChannels();
                            cout <<"): ";
                            int index;
                            cin >> index;
                            channelManager.setCurrentChannel(index);
                            fflush(stdin);
                            break;
                        }
                        case 'a': {
                            channelManager.updateChannel();
                            fflush(stdin);
                            break;
                        }
                        case 'w':
                            fflush(stdin);
                            break;
                        default:
                            cout << "Nieprawidłowy wybór. Spróbuj ponownie." << endl;
                    }
                }while(choice!='w');
                system("clear");
                fflush(stdin);
                break;
            }

            case '8': {
                char sortOption, orderOption;

                cout << "Sortuj po (n)azwie czy (i)lości filmów? ";
                cin >> sortOption;

                switch (sortOption) {
                    case 'n': {
                        do {
                            fflush(stdin);
                            cout << "Kolejność (a)lfabetyczna czy (o)dwrotnie? ";
                            cin >> orderOption;
                            if (orderOption != 'a' && orderOption != 'o') {
                                cout << "Błąd: Nieprawidłowy wybór kolejności." << endl;
                                continue;
                            }
                        } while (orderOption != 'a' && orderOption != 'o');
                        break;
                    }

                    case 'i': {
                        do {
                            fflush(stdin);
                            cout << "Kolejność (r)osnąca czy (m)alejąca? ";
                            cin >> orderOption;

                            if (orderOption != 'r' && orderOption != 'm') {
                                cout << "Błąd: Nieprawidłowy wybór kolejności." << endl;
                                continue;
                            }
                        } while (orderOption != 'r' && orderOption != 'm');
                        break;
                    }

                    default:
                        cout << "Błąd: Nieprawidłowy wybór kolejności." << endl;
                        fflush(stdin);
                }



                channelManager.sortChannels(sortOption, orderOption);
                cout << "Posortowano." << endl;
                channelManager.displayAllChannels();

                break;
            }
            case '9': {
                cout<< "Wyszukiwanie po:\n"
                       "n. Nazwie kanału\n"
                       "l. liczbie filmów\n"
                       "w. Wróć do strony głównej: ";
                char i;
                cin>>i;
                switch (i) {
                    case 'l':
                        system("clear");
                        do {
                            cout << "Podaj przedział minimalny: " << endl;
                            cin >> min;
                            cout << "Podaj przedział maksymalny: " << endl;
                            cin >> max;
                            if (min > max) {
                                cout << "Wartość minimalna nie może być wieksza od wartosci maksymalnej!" << endl;

                            }
                        }while(min>max);
                        channelManager.searchChannelsByVid(max,min);
                        if (channelManager.getFoundChannelsByVid().empty()) {
                            cout << "Nie znaleziono dopasowanych kanałów" << endl;
                        } else {
                            char wybor;
                            do {
                                channelManager.displaySearchResultsByVid(channelManager.getFoundChannelsByVid());
                                cout << " Następny (n)\n Poprzedni (p)\n Wróć do strony głównej (w): ";
                                cin >> wybor;

                                switch (wybor) {
                                    case 'n':
                                        system("clear");
                                        channelManager.nextFoundByVid();
                                        fflush(stdin);
                                        break;
                                    case 'p':
                                        system("clear");
                                        channelManager.previousFoundByVid();
                                        fflush(stdin);
                                        break;
                                    case 'w':
                                        break;
                                    default:
                                        cout << "Nieprawidłowy wybór. Spróbuj ponownie." << endl;
                                        fflush(stdin);
                                }
                            } while (wybor != 'w');
                        }
                        fflush(stdin);
                        break;
                    case 'n':
                        cout << "Podaj litery do wyszukania: ";
                        cin >> lettersToSearch;

                        channelManager.searchChannelsByLetters(lettersToSearch);

                        if (channelManager.getFoundChannels().empty()) {
                            cout << "Nie znaleziono dopasowanych kanałów" << endl;
                        } else {
                            char choice;
                            do {
                                channelManager.displaySearchResults(channelManager.getFoundChannels());
                                cout << " Następny (n)\n Poprzedni (p)\n Wróć do strony głównej (w): ";
                                cin >> choice;

                                switch (choice) {
                                    case 'n':
                                        system("clear");
                                        channelManager.nextFound();
                                        fflush(stdin);
                                        break;
                                    case 'p':
                                        system("clear");
                                        channelManager.previousFound();
                                        fflush(stdin);
                                        break;
                                    case 'w':
                                        break;
                                    default:
                                        cout << "Nieprawidłowy wybór. Spróbuj ponownie." << endl;
                                        fflush(stdin);
                                }
                            } while (choice != 'w');
                        }
                        fflush(stdin);
                        break;
                    case 'w':
                        break;
                    default:
                        cout << "Nieprawidłowy wybór. Spróbuj ponownie." << endl;
                        fflush(stdin);
                }
            }
                fflush(stdin);
                break;
            case 'q':
            {
                char confirmation;
                cout << "Czy na pewno chcesz usunąć wszystkie dane? (t/n): ";
                cin >> confirmation;

                if (confirmation == 't' || confirmation == 'T') {
                    channelManager.clearAllData();
                    cout<<"Usunieto wszystkie dane"<<endl;
                    fflush(stdin);
                } else {
                    cout << "Operacja anulowana." << endl;
                }
                break;
            }

            case 'k': {
                char ch;
                do {
                    cout<<"\n";
                    channelManager.displayTrashChannels();
                    cout << "\n Następny (n)\n Poprzedni (p)\n "
                            "Przywróć kanał (r)\n Usuń bieżący kanał (u)\n "
                            "Wyświetl całą zawartość (a)\n Wyczyszcz cały kosz (c)\n Wróć (w): ";
                    cin >> ch;

                    switch (ch) {
                        case 'n':
                            system("clear");
                            channelManager.nextTrash();
                            fflush(stdin);
                            break;
                        case 'p':
                            system("clear");
                            channelManager.previousTrash();
                            fflush(stdin);
                            break;
                        case 'a':
                            system("clear");
                            channelManager.displayAllTrash();
                            fflush(stdin);
                            break;
                        case 'r':
                            system("clear");
                            channelManager.displayTrashChannels();
                            channelManager.restoreChannelFromTrash();
                            fflush(stdin);
                            break;
                        case 'u':
                            system("clear");
                            channelManager.displayTrashChannels();
                            cout << "Czy na pewno chcesz usunąć bieżący kanał? (T/N): ";
                            char confirm;
                            cin >> confirm;
                            if (confirm == 'T' || confirm == 't') {
                                channelManager.deleteCurrentTrash();
                                fflush(stdin);
                            } else
                                cout << "Nie usunięto kanału." << endl;
                            break;
                        case 'c':
                            system("clear");
                            cout << "Czy na pewno chcesz wyczyscic kosz? (t/n): ";
                            char confirmation;
                            cin>>confirmation;

                            if (confirmation == 'T' || confirmation == 't') {
                                channelManager.clearTrashChannels();
                                cout << "Kosz wyczyszczono." << endl;
                            } else
                                cout << "Operacja czyszczenia anulowana." << endl;

                            fflush(stdin);
                            break;
                        case 'w':
                            fflush(stdin);
                            break;
                        default:
                            cout << "Nieprawidłowy wybór. Spróbuj ponownie." << endl;
                            fflush(stdin);
                    }
                } while (ch != 'w');
                system("clear");
                break;
            }
            case 'e':
            {
                cout << "Czy na pewno chcesz Wyjsc? (T/N): ";
                char conf;
                cin >> conf;
                if (conf == 'T' || conf == 't') {
                    zn = '0';
                    cout << "do widzenia!" << endl;
                    getchar();
                }
                fflush(stdin);
                break;
            }
            default:
                cout << "Nieprawidłowy wybór. Spróbuj ponownie." << endl;
                fflush(stdin);
        }
    }
    while(zn!='0');
}
