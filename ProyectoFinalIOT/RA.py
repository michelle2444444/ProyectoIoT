import cv2
import numpy as np
import cv2.aruco as aruco
import requests

# 📌 Credenciales de Blynk
TOKEN = "NMI9BorKtYi53_qUGxuv0106zGmU-J_l"
PIN_LUZ = 'V9'  # FotoResistencia
PIN_PATIO = 'V15'  # Estado de la luz del Patio

# URLs de Blynk
BLYNK_API_URL = f'https://blynk.cloud/external/api/get?token={TOKEN}&'

# 📌 Función para obtener el valor de un pin en Blynk
def obtener_valor_blynk(pin):
    try:
        respuesta = requests.get(f"{BLYNK_API_URL}{pin}")
        if respuesta.status_code == 200:
            return int(respuesta.text)  # Convertir la respuesta a entero
        else:
            print(f"⚠️ Error en la solicitud a Blynk: {respuesta.status_code}")
            return None
    except Exception as e:
        print(f"⚠️ Error en la conexión: {e}")
        return None

# 📌 Configuración de OpenCV y ArUco
parametros = aruco.DetectorParameters()
diccionario_aruco = aruco.getPredefinedDictionary(aruco.DICT_7X7_100)

# 📌 Cargar imágenes del foco
imagen_foco_apagado = cv2.imread("focoApagado.png")
imagen_foco_encendido = cv2.imread("focoPrendido.png")

# 📌 Iniciar la cámara
captura = cv2.VideoCapture(0)

while True:
    # Capturar el frame de la cámara
    lectura, frame = captura.read()
    
    # Convertir a escala de grises
    frame_gris = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    
    # Detectar marcadores ArUco
    detector = aruco.ArucoDetector(diccionario_aruco, parametros)
    esquinas, ids, _ = detector.detectMarkers(frame_gris)

    # 📌 Obtener datos desde Blynk
    valor_luz = obtener_valor_blynk(PIN_LUZ)  # FotoResistencia

    # 📌 Determinar qué imagen usar según la fotoresistencia
    if valor_luz is not None and valor_luz < 40:
        imagen_foco = imagen_foco_encendido
    else:
        imagen_foco = imagen_foco_apagado

    if ids is not None:
        # Dibujar los marcadores detectados
        aruco.drawDetectedMarkers(frame, esquinas, ids)

        for i in range(len(ids)):
            # Obtener las esquinas del marcador
            x, y, w, h = cv2.boundingRect(esquinas[i][0])
            
            # 📌 Ajustar tamaño y superponer imagen en el marcador detectado
            if imagen_foco is not None:
                imagen_resized = cv2.resize(imagen_foco, (w, h))
                frame[y:y+h, x:x+w] = imagen_resized

            # 📌 Mostrar información en pantalla
            cv2.putText(frame, f"FotoResistencia: {valor_luz}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2)

    # 📌 Mostrar el resultado en pantalla
    cv2.imshow('Realidad Aumentada - Blynk', frame)

    # Salir con la tecla 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 📌 Liberar la cámara y cerrar ventanas
captura.release()
cv2.destroyAllWindows()
