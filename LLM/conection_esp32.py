# Script para acceder al endpoint 
import requests
import time


def data_request():
    # La dirección IP del ESP32
    url = 'http://192.168.1.189'
    
    try:
        # Realiza una solicitud GET al servidor
        response = requests.get(url)
        
        # Verifica si la solicitud fue exitosa (código de estado 200)
        if response.status_code == 200:
            # Imprime el contenido de la respuesta
            print(f'Estatus: {response.status_code}')
            print(f'Sensores: {response.text}')
            
            return response.text
        else:
            print(f'Error: Código de estado {response.status_code}')
    except requests.exceptions.RequestException as e:
        # Maneja excepciones que pueden ocurrir al hacer la solicitud
        print('Error al conectar al ESP32:', e)
        

if __name__ == "__main__":
    data_request()