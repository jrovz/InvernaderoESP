# Script para acceder al endpoint 
import requests
import time
import pdb
# La dirección IP del ESP32
url = 'http://192.168.1.108'

def parameters():
    time.sleep(1)
    try:
        # Realiza una solicitud GET al servidor
        response = requests.get(url)
        
        # Verifica si la solicitud fue exitosa (código de estado 200)
        if response.status_code == 200:
            # Imprime el contenido de la respuesta
            print('\nRespuesta recibida del ESP32:')
            print(response.text)
            return response.text
        
        else:
            print(f'Error: Código de estado {response.status_code}')
    except requests.exceptions.RequestException as e:
        # Maneja excepciones que pueden ocurrir al hacer la solicitud
        print('Error al conectar al ESP32:', e)
    
parameters()