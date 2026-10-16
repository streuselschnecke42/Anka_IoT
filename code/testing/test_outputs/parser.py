import requests


def formatToJson(timestamp: str, serialData: str):
    """
    CREATE JSON FORMATTED STRING\n
    SPLITS SERIAL PRINTOUT INTO SNIPPETS
    & INSERTS SNIPPETS IN JSON FORMAT\n
    TIMESTAMP GETS INSERTED FIRST BUT IS SEPARATE INPUT
    """

    dataSnippets = serialData.split(":")
    dataJson = {
        "timestamp": timestamp,
        "msgtype": dataSnippets[0],
        "module": dataSnippets[1],
        "type": dataSnippets[2],
        "subtype": dataSnippets[3],
        "status": dataSnippets[4],
        "slot": dataSnippets[5],
        "value": dataSnippets[6],
        "mode": dataSnippets[7],
        "modeinfo": dataSnippets[8]
    }

    return dataJson


def sendData(data: str):
    """
    SENDS PARSED FILE TO NODE.JS VIA POST REQUEST
    """

    requests.post("http://localhost:2006/logger", json=data)
