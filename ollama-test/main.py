from ollama import chat

response = chat(model="deepseek-r1:1.5b", messages=[
    { "role": "user", "content": "Why is the sky blue?", }
])

print(response.message.content)
