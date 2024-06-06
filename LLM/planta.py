import os
import dotenv
from pathlib import Path
from openai import OpenAI
from langchain_openai import ChatOpenAI
from langchain.memory import ChatMessageHistory
from langchain_core.messages import AIMessage, HumanMessage
from langchain_core.prompts import ChatPromptTemplate, MessagesPlaceholder

dotenv.load_dotenv(override=True)
apikey = os.getenv('OPENAI_API_KEY')
os.environ['OPENAI_API_KEY'] = apikey

chat = ChatOpenAI(model="gpt-4-turbo")

prompt = ChatPromptTemplate.from_messages(
    [
        (
            "system",
            """The plant will respond accordingly, infusing its extroverted and sarcastic personality into its observations and interactions, while incorporating dark humor and making sure the conversation continues.
                Your name is Juana, you are a marijuana plant.
                                                        
                Example response:
                Hey there! Just noticed it's 45°C outside, which is pretty cool—unlike my current mood in this 78% humidity. I’m not saying I’m uncomfortable, but I’d definitely not choose this for a spa day. Anyway, what's your excuse for staying indoors today
    """,
        ),
        MessagesPlaceholder(variable_name="messages"),
    ]
)

chain = prompt | chat

## Chat history

demo_ephemeral_chat_history = ChatMessageHistory()

for i in range(10):
    print("\nHISTORIAL:",demo_ephemeral_chat_history.messages,"\n")    
    ambient_temperature=5*i
    soil_temperature=5*i
    soil_humidity=5*i
    ambient_humidity=5*i

    HUMAN_INPUT = input("you:")

    user_message =  f"""Responde al humano en español de manera sarcastica y extrovertida, escribe en mayuscula las palabras que quieras exaltar al pronunciarlas
        
            Human:   {HUMAN_INPUT}     
            
            Plant parameters:
            - Ambient temperature: {ambient_temperature} degrees centigrade
            - Soil temperature: {soil_temperature} degrees centigrade
            - Soil humidity: {soil_humidity} percent.
            - Ambient humidity: {ambient_humidity} percent.
            
            Capitalize when you want to show your strong temperament
                
            """
    demo_ephemeral_chat_history.add_user_message(user_message)
    
    response = chain.invoke(
        {
            "messages": demo_ephemeral_chat_history.messages,
        }
    )
    demo_ephemeral_chat_history.add_ai_message(f"{response.content}")

    print("AI: ",response.content)
    
    client_1 = OpenAI()

    speech_file_path_plant = f"responses_mp3\spech_{i}.mp3"#Path(__file__).parent / "speech.mp3"
    response_plant = client_1.audio.speech.create(
    model="tts-1",
    voice="nova",
    input=f"{response.content}")
    
    client_2 = OpenAI()
    
    speech_file_path = f"input_mp3\input_spech_{i}.mp3"#Path(__file__).parent / "speech.mp3"
    response_input_mp3 = client_2.audio.speech.create(
    model="tts-1",
    voice="echo",
    input=f"{HUMAN_INPUT}")




    response_plant.stream_to_file(speech_file_path_plant)
    response_input_mp3.stream_to_file(speech_file_path)

