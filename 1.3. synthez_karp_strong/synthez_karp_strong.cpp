//========подключение библиотек========//
#include <stdio.h> 
#include <iostream>
#include <memory.h>  // библиотека работы с памятью
#include <inttypes.h>
#include <vector>
#include <cmath>

// ============= определение псевдонимов для типов данных =======//
typedef uint16_t    uint16;
typedef uint32_t    uint32;
typedef int16_t     int16;
typedef int32_t     int32;

//============== Структура создания заголовка wav-файла ===============//
struct SMinimalWaveFileHeader
{
    //========Основной фрагмент ========//
    unsigned char m_chunkID[4];
    uint32        m_chunkSize;
    unsigned char m_format[4];
    //=========внутренний фрагмент формата звуковых данных (fmt)==========//
    unsigned char m_subChunk1ID[4];
    uint32        m_subChunk1Size;   //размер первого внутреннего фрагмента
    uint16        m_audioFormat;
    uint16        m_numChannels;     //количество каналов
    uint32        m_sampleRate;      // частота дискретизации
    uint32        m_byteRate;        // скорость передачи данных 
    uint16        m_blockAlign;    
    uint16        m_bitsPerSample;   // количество бит на сэмпл
    //========второй внутренний фрагмент==========//
    unsigned char m_subChunk2ID[4];
    uint32        m_subChunk2Size;
};

//==============Запись данных в WAV-файл=================//
template <typename T>
bool WriteWaveFile(const char *fileName, std::vector<T> data, int16 numChannels, int32 sampleRate)
{
    int32 dataSize = data.size() * sizeof(T);
    int32 bitsPerSample = sizeof(T) * 8;      //количество бит на семпл
    //открытие файла в бинарном режиме
    FILE *File = nullptr;
    fopen_s(&File, fileName, "w+b");
    if (!File)
        return false;
    SMinimalWaveFileHeader waveHeader;
    //=======заполнение основного фрагмента============//
    memcpy(waveHeader.m_chunkID, "RIFF", 4);   //копирование 4 байт из "RIFF" в waveHeader.m_chunkID
    waveHeader.m_chunkSize = dataSize + 36;    //определение размера основного фрагмента
    memcpy(waveHeader.m_format, "WAVE", 4);    //копирование 4 байт из "WAVE" в waveHeader.m_format
    //=============заполнение внутреннего фрагмента формата "fmt" =====================//
    memcpy(waveHeader.m_subChunk1ID, "fmt ", 4);   //копирование 4 байт из "fmt" в waveHeader.m_subChunk1ID
    waveHeader.m_subChunk1Size = 16;               //определение размера первого внутреннего фрагмента
    waveHeader.m_audioFormat = 1;                  // 
    waveHeader.m_numChannels = numChannels;        //количество каналов
    waveHeader.m_sampleRate = sampleRate;          // частота дискретизации
    waveHeader.m_byteRate = sampleRate * numChannels * bitsPerSample / 8;     // скорость передачи данных 
    waveHeader.m_blockAlign = numChannels * bitsPerSample / 8;
    waveHeader.m_bitsPerSample = bitsPerSample;
    //=============заполнение второго внутреннего фрагмента "data" =====================//
    memcpy(waveHeader.m_subChunk2ID, "data", 4);     //копирование 4 байт из "data" в waveHeader.m_subChunk2ID
    waveHeader.m_subChunk2Size = dataSize;           //определение размера второго внутреннего фрагмента
    //=====запись заголовка=====//
    fwrite(&waveHeader, sizeof(SMinimalWaveFileHeader), 1, File);  
    //========запись основных данных ==========//
    fwrite(&data[0], dataSize, 1, File);
    fclose(File);  //закрытие файла
    return true;   //возврат результата записи
}


//==========Функция преобразования вектора типа float в вектор необходимого типа=============//
template <typename T>
void ConvertFloatSamples (const std::vector<float>& in, std::vector<T>& out)
{
    out.resize(in.size());                //изменение размера вектора out до размера вектора in
    //=======преобразование элементов входного вектора в элементы выходного вектора==========//
    for (size_t i = 0, c = in.size(); i < c; ++i)
    {
        float v = in[i];                                      //получение элемента вектора in
        if (v < 0.0f)                                         //если значение отрицательное, то...
            v *= -float(std::numeric_limits<T>::lowest());    //...значение элемента вектора умножается на 
                                                              //отрицательный минимальный предел для типа Т
        else                                                  //...иначе, если значение положительное..
            v *= float(std::numeric_limits<T>::max());        //..умножает значение на максимальный предел для типа Т
        out[i] = T(v);                                        // преобразованное значение присваивается элементу вектора out
    }
}


// =================Алгоритм синтеза частоты методом Карплуса-Стронга==================//
class CKarplusStrongStringPluck
{
public:
    CKarplusStrongStringPluck (float frequency, float sampleRate, float feedback)
    {
        m_buffer.resize(uint32(float(sampleRate) / frequency));      //инициализация буфера сэмплов размером частота (дискретизации/частота ноты)
        //==заполнение буфера случайным шумом==//
        for (size_t i = 0, c = m_buffer.size(); i < c; ++i) {
            m_buffer[i] = ((float)rand()) / ((float)RAND_MAX) * 2.0f - 1.0f;  
        }
        m_index = 0;
        m_feedback = feedback;   // Инициализация коэффициента обратной связи
    }
    //=====метод, осуществляющий генерацию следующего сэмпла======//
    float GenerateSample()
    {
        // получение текущего сэмпла
        float ret = m_buffer[m_index];
        // вычисление среднего значения текущего и предыдущего сэмплов, умножение на коэф.обратной связи
        float value = (m_buffer[m_index] + m_buffer[(m_index + 1) % m_buffer.size()]) * 0.5f * m_feedback;
        m_buffer[m_index] = value;    // запись вычисленного значения в буфер сэмплов
        // перемещение индекса буфера сэмплов на следующий сэмпл
        m_index = (m_index + 1) % m_buffer.size();
        // возвращение сэмпла из буфера
        return ret;
    }
private:
    std::vector<float>  m_buffer;
    size_t              m_index;
    float               m_feedback;
};


//======Алгоритм генерации сэмплов =======//
void GenerateSamples (std::vector<float>& samples, int sampleRate, std::vector<float>& frequencies, float c_damp){
    std::vector<CKarplusStrongStringPluck> notes;      //объявление вектора нот

    int timeBegin = 0;                    // установка начального времени
   
    //============Алгоритм генерации сэмплов============//
    for (int index = 0, numSamples = samples.size(); index < numSamples; ++index)
    {
        const int c_noteTime = sampleRate / 2;
        int time = index - timeBegin;                 // вычисление времени, прошедшего с начала
        
        if (time % c_noteTime == 0) {
            int note = time / c_noteTime;          // вычисление номера ноты
            notes.push_back(CKarplusStrongStringPluck(frequencies[0], float(sampleRate), c_damp));
        }

        // генерация и объединение сэмплов из всех нот
        samples[index] = 0;

        for (CKarplusStrongStringPluck& note : notes)
            samples[index] += note.GenerateSample();
        // =====ограничение сэмплов для предотвращения клиппинга=====//
        samples[index] *= 0.5f;
    }
}


//=========ОСНОВНОЙ АЛГОРИТМ =========//
int main(int argc, char **argv){
    
    std::vector<float> userFrequencies;             //создание вектора для частот, которые задаёт пользователь

    //----пользовательский ввод длительности звуковой дорожки----//
    int c_numSeconds;
    printf("Enter audio track duration:");
    scanf("%d", &c_numSeconds);

    //----пользовательский ввод частоты дискретизации----//
    int c_sampleRate;
    printf("Enter audio track sampling rate:");
    scanf("%d", &c_sampleRate);

    //----пользовательский ввод коэффициента затухания звуковой дорожки----//
    float c_damp;
    printf("Enter damping coefficient:");
    scanf("%f", &c_damp);

    // ------Ввод названия аудиофайла------//
    char audio_name[256];
    printf("Enter audio file name:");
    scanf("%s", audio_name);

    //====ввод количества необходимых пользователю частот=====//
    int numFrequencies;
    printf("Enter the number of frequencies you want to play: ");
    scanf("%d", &numFrequencies);
    
   //=====ввод пользовательских частот=======//
    for (int i = 0; i < numFrequencies; i++) {
        float frequency;
        printf("Enter frequency %d (in Hz): ", i + 1);
        scanf("%f", &frequency);
        userFrequencies.push_back(frequency);
    }


    //-----задание параметров звуковой дорожки-----//
    const int c_numChannels = 1;       // количество каналов (1 - моно)
    const int c_numSamples = c_sampleRate * c_numChannels * c_numSeconds; //количество сэмплов

    //------инициализация вектора для хранения сэмплов-------//
    std::vector<float> samples;
    samples.resize(c_numSamples);  // изменение размера вектора сэмплов до вычисленного количества сэмплов
    
    //----вызов функции, котрая генерирует сэмплы и заполняет ими вектор сэмплов
    GenerateSamples(samples, c_sampleRate, userFrequencies, c_damp);

    //----объявление вектора для хранения сэмплов в формате int16
    std::vector<int16> samplesInt;
    ConvertFloatSamples(samples, samplesInt);   // преобразование вектора из типа float в int16

    // ====Сохранение полученного файла=====//
    WriteWaveFile(audio_name, samplesInt, c_numChannels, c_sampleRate);
    return 0;
}
