#! /bin/python3
import os
import sys
from dotenv import load_dotenv
from pyyoutube import Api 

def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} [video ID]")

    video_id = sys.argv[1]

    load_dotenv()
    YOUTUBE_API_KEY = os.getenv("YOUTUBE_API_KEY")
    
    api = Api(api_key=YOUTUBE_API_KEY)

    video = api.get_video_by_id(video_id=video_id)
    print(video.items[0].to_json())

if __name__ == "__main__":
    main()
