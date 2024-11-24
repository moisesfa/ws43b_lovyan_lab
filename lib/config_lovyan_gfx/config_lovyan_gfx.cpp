#include <LovyanGFX.hpp>
#include "config_lovyan_gfx.h"
#include "screen_01.h"
#include "screen_02.h"
#include "screen_all.h"

// Creamos el objeto
LGFX tft;

// Valores configurables para los umbrales de detección
#define GESTURE_THRESHOLD_X 40 // Desplazamiento mínimo en X para un gesto
#define GESTURE_THRESHOLD_Y 40 // Desplazamiento mínimo en Y para un gesto

// Cola para comunicar gestos
QueueHandle_t gestureQueue;
// Cola para comunicar tiempo
QueueHandle_t timeQueue;

// Enumeración para almacenar gestos
enum Gesture
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};
// Estructura para almacenar coordenadas
struct TouchData
{
    int16_t x;
    int16_t y;
    bool touched;
};

struct TimeData
{
    uint8_t ho;
    uint8_t mi;
    uint8_t se;
};

// Tarea para leer coordenadas táctiles y detectar gestos
void taskReadTouchAndDetectGesture(void *pvParameters)
{
    lgfx::touch_point_t touch;
    TouchData touchData;
    int16_t prevX = -1, prevY = -1;
    Gesture detectedGesture = NONE;

    while (true)
    {
        touchData.touched = tft.getTouch(&touch); // Verifica si hay toque
        if (touchData.touched)
        {
            touchData.x = touch.x;
            touchData.y = touch.y;
            Serial.printf("X:%d Y:%d\n", touch.x, touch.y);

            // Detectar gestos si hay una posición previa válida
            if (prevX != -1 && prevY != -1)
            {
                int16_t deltaX = touchData.x - prevX;
                int16_t deltaY = touchData.y - prevY;

                if (abs(deltaX) > abs(deltaY))
                { // Predomina el movimiento horizontal
                    if (deltaX > GESTURE_THRESHOLD_X)
                    {
                        detectedGesture = RIGHT;
                    }
                    else if (deltaX < -GESTURE_THRESHOLD_X)
                    {
                        detectedGesture = LEFT;
                    }
                }
                else
                { // Predomina el movimiento vertical
                    if (deltaY > GESTURE_THRESHOLD_Y)
                    {
                        detectedGesture = DOWN;
                    }
                    else if (deltaY < -GESTURE_THRESHOLD_Y)
                    {
                        detectedGesture = UP;
                    }
                }
            }

            // Guardar las coordenadas actuales como referencia
            prevX = touchData.x;
            prevY = touchData.y;
        }
        else
        {
            // Si no hay toque, reiniciar coordenadas previas
            prevX = -1;
            prevY = -1;
        }

        // Enviar gesto detectado a la cola
        xQueueSend(gestureQueue, &detectedGesture, portMAX_DELAY);

        // Resetear gesto después de enviar
        detectedGesture = NONE;

        vTaskDelay(pdMS_TO_TICKS(50)); // Leer cada 50 ms
    }
}

// Tarea para gestionar los gestos
void taskManagementGesture(void *pvParameters)
{
    Gesture gesture;

    while (true)
    {
        // Recibir datos de la cola
        if (xQueueReceive(gestureQueue, &gesture, portMAX_DELAY))
        {
            switch (gesture)
            {
            case UP:
                Serial.println("Gesture: UP");
                break;
            case DOWN:
                Serial.println("Gesture: DOWN");
                break;
            case LEFT:
                if (screen_number != 2)
                    screen_02_view();
                Serial.println("Gesture: LEFT");
                break;
            case RIGHT:
                if (screen_number != 1)
                    screen_01_view();
                Serial.println("Gesture: RIGHT");
                break;
            case NONE:
            default:
                break;
            }
        }
    }
}

void taskManagementTime(void *pvParameters)
{

    static int8_t hour = 0;
    static int8_t minute = 0;
    static int8_t second = 0;
    TimeData timedata;

    while (1)
    {
        second++;
        if (second > 59)
        {
            second = 0;
            minute++;
            if (minute > 59)
            {
                minute = 0;
                hour++;
                if (hour > 23)
                {
                    hour = 0;
                }
            }
        }
        // LLenamos la estructura
        timedata.ho = hour;
        timedata.mi = minute;
        timedata.se = second;

        if (xQueueSend(timeQueue, &timedata, pdMS_TO_TICKS(1000)) == pdPASS)
        {
            Serial.printf("Mensaje enviado: h:%i m:%i s:%i\n", timedata.ho, timedata.mi, timedata.se);
        }
        else
        {
            Serial.printf("Error: Cola llena.\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void taskShowTimeOnScreen(void *pvParameters)
{
    char *mensajeRecibido;
    TimeData timedata;

    while (true)
    {
        if (xQueueReceive(timeQueue, &timedata, pdMS_TO_TICKS(1000)) == pdPASS)
        {
            Serial.printf("Mensaje recibido: h:%i m:%i s:%i\n", timedata.ho, timedata.mi, timedata.se);
            String hour_time_f = (timedata.ho < 10) ? "0" + String(timedata.ho) : String(timedata.ho);
            String minute_time_f = (timedata.mi < 10) ? "0" + String(timedata.mi) : String(timedata.mi);
            String second_time_f = (timedata.se < 10) ? "0" + String(timedata.se) : String(timedata.se);
            String rec_time_str = String(hour_time_f) + ":" + String(minute_time_f) + ":" + String(second_time_f);
            if (screen_number == 1)
                screen_01_view_time(rec_time_str);
        }
        else
        {
            Serial.printf("Error: No se recibió ningún mensaje.\n");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void lovyangfx_init()
{
    Serial.println("Lgfx init...");

    tft.init();
    // TODO: Crear una tarea que lea constantemente el touch
    //  Crear tarea FreeRTOS para leer el touch
    //  tft.setRotation(0);
    screen_01_view();

    // Crear cola para los gestos
    gestureQueue = xQueueCreate(5, sizeof(Gesture));
    if (gestureQueue == NULL)
    {
        Serial.println("Error al crear cola");
        // tft.println("Error al crear cola");
        while (true)
            ; // Detener en caso de error
    }

    // Crear cola con capacidad para 5 elementos del tamaño de un puntero
    timeQueue = xQueueCreate(5, sizeof(TimeData));
    if (timeQueue == NULL)
    {
        Serial.println("Error al crear cola");
        while (true)
            ; // Detener en caso de error
    }

    // Crear tareas FreeRTOS
    xTaskCreate(taskReadTouchAndDetectGesture, "TaskReadTouchAndDetectGesture", 1024 * 2, NULL, 1, NULL);
    xTaskCreate(taskManagementGesture, "TaskManagementGesture", 1024 * 2, NULL, 1, NULL);
    xTaskCreate(taskShowTimeOnScreen, "TaskShowTimeOnScreen", 1024 * 2, NULL, 1, NULL);
    xTaskCreate(taskManagementTime, "TaskManagementTime", 1024 * 2, NULL, 1, NULL);
}

// Only if GFX does the touch handling
void LgfxDoTouch()
{
    int32_t x, y;
    if (tft.getTouch(&x, &y))
    {
        Serial.printf("X:%d Y:%d\n", x, y);
        tft.fillCircle(x, y, 15, TFT_GREEN);
    }
}