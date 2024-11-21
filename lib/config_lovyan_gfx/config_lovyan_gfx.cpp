#include <LovyanGFX.hpp>
#include "config_lovyan_gfx.h"
#include "screen_01.h"

// Creamos el objeto
LGFX tft;

// Valores configurables para los umbrales de detección
#define GESTURE_THRESHOLD_X 40 // Desplazamiento mínimo en X para un gesto
#define GESTURE_THRESHOLD_Y 40 // Desplazamiento mínimo en Y para un gesto

// Cola para comunicar gestos
QueueHandle_t gestureQueue;
// Enumeración para almacenar gestos
enum Gesture { NONE, UP, DOWN, LEFT, RIGHT };
// Estructura para almacenar coordenadas
struct TouchData {
    int16_t x;
    int16_t y;
    bool touched;
};

// Tarea para leer coordenadas táctiles y detectar gestos
void taskReadTouchAndDetectGesture(void* pvParameters) {
    lgfx::touch_point_t touch;
    TouchData touchData;
    int16_t prevX = -1, prevY = -1;
    Gesture detectedGesture = NONE;

    while (true) {
        touchData.touched = tft.getTouch(&touch); // Verifica si hay toque
        if (touchData.touched) {
            touchData.x = touch.x;
            touchData.y = touch.y;
            Serial.printf("X:%d Y:%d\n", touch.x, touch.y);

            // Detectar gestos si hay una posición previa válida
            if (prevX != -1 && prevY != -1) {
                int16_t deltaX = touchData.x - prevX;
                int16_t deltaY = touchData.y - prevY;

                if (abs(deltaX) > abs(deltaY)) { // Predomina el movimiento horizontal
                    if (deltaX > GESTURE_THRESHOLD_X) {
                        detectedGesture = RIGHT;
                    } else if (deltaX < -GESTURE_THRESHOLD_X) {
                        detectedGesture = LEFT;
                    }
                } else { // Predomina el movimiento vertical
                    if (deltaY > GESTURE_THRESHOLD_Y) {
                        detectedGesture = DOWN;
                    } else if (deltaY < -GESTURE_THRESHOLD_Y) {
                        detectedGesture = UP;
                    }
                }
            }

            // Guardar las coordenadas actuales como referencia
            prevX = touchData.x;
            prevY = touchData.y;
        } else {
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

// Tarea para mostrar gestos en la pantalla
void taskShowGestureOnScreen(void* pvParameters) {
    Gesture gesture;

    while (true) {
        // Recibir datos de la cola
        if (xQueueReceive(gestureQueue, &gesture, portMAX_DELAY)) {
            //tft.fillScreen(TFT_BLACK); // Limpiar la pantalla
            // Borrar solo el área del texto usando un rectángulo
            tft.fillRect(200, 100, 400, 100, 0x003030);

            //tft.setTextColor(TFT_WHITE);
            tft.setTextDatum(CC_DATUM);    
            tft.setTextSize(2);
            tft.setCursor(400, 100);

            switch (gesture) {
                case UP:
                    tft.drawString("Gesture: UP",400,150);
                    //tft.println("Gesture: UP");
                    break;
                case DOWN:
                    tft.drawString("Gesture: DOWN",400,150);
                    //tft.println("Gesture: DOWN");
                    break;
                case LEFT:
                    tft.drawString("Gesture: LEFT",400,150);
                    //tft.println("Gesture: LEFT");
                    break;
                case RIGHT:
                    tft.drawString("Gesture: RIGHT",400,150);
                    //tft.println("Gesture: RIGHT");
                    break;
                case NONE:
                default:
                    //tft.println("No gesture");
                    break;
            }
        }
    }
}


void lovyangfx_init() {
  Serial.println("Lgfx init...");

  tft.init();
  //TODO: Crear una tarea que lea constantemente el touch
  // Crear tarea FreeRTOS para leer el touch
  // tft.setRotation(0);
  screen_01_view();

  // Crear cola para los gestos
    gestureQueue = xQueueCreate(5, sizeof(Gesture));
    if (gestureQueue == NULL) {
        tft.println("Error al crear cola");
        while (true); // Detener en caso de error
    }

    // Crear tareas FreeRTOS
    xTaskCreate(taskReadTouchAndDetectGesture, "TaskReadTouchAndDetectGesture", 1024*2, NULL, 1, NULL);
    xTaskCreate(taskShowGestureOnScreen, "TaskShowGestureOnScreen", 1023*2, NULL, 1, NULL);
}

// Only if GFX does the touch handling
void LgfxDoTouch() {
  int32_t x, y;
  if (tft.getTouch(&x, &y)) {
    Serial.printf("X:%d Y:%d\n", x, y);
    tft.fillCircle(x, y, 15, TFT_GREEN);
  }
}