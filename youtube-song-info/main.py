#! /bin/python3
import os
import sys
import json
from dotenv import load_dotenv
from pyyoutube import Api 

def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} [video ID]")
        return

    video_id = sys.argv[1]

    load_dotenv()
    YOUTUBE_API_KEY = os.getenv("YOUTUBE_API_KEY")
    
    api = Api(api_key=YOUTUBE_API_KEY)

    video = api.get_video_by_id(video_id=video_id)
    video_json = video.items[0].to_json()

    video = json.loads(video_json)

    video_title = video["snippet"]["title"]
    video_desc = video["snippet"]["description"]

    print(f"Title: \"{video_title}\"")
    print(f"Description:\n{video_desc}")

if __name__ == "__main__":
    main()
