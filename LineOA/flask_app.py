from flask import Flask, request, send_from_directory
from werkzeug.middleware.proxy_fix import ProxyFix
from linebot import LineBotApi, WebhookHandler
from linebot.models import (
    MessageEvent, TextMessage, TextSendMessage,
    ImageMessage, ImageSendMessage,
    TemplateSendMessage, MessageAction,
    ButtonsTemplate, URIAction, TemplateSendMessage, MessageAction,
    ImageCarouselTemplate, ImageCarouselColumn, QuickReply, ConfirmTemplate)
import random
import os
import tempfile
import cv2
import numpy as np
from wit import Wit
from openpyxl import load_workbook
from yolo_predictions import YOLO_Pred            
import json

yolo = YOLO_Pred('my_obj.onnx','my_obj.yaml') #อ่านไฟล์เทรนในเครื่อง



channel_secret = "f158a1c9f11754622c5e5c91ae56addf"#Token Line หน้าบ้านไปหลังบ้าน
channel_access_token = "rKNcBkF1dLR+3Bf2XWP6Avt0OWrLAcqGATqpHDRxo9AzZGWxABW7FGGh7L/+rmRAGuihe8no3Xf0CKs7v6szkfNoJ26w1tcULTCboOzDvWish+uXkPBwjNnguIF4oXE6PqbH0S4LwaUn8Lo0NL8guQdB04t89/1O/w1cDnyilFU=" #Token Lineหลังบ้านไปหน้าบ้าน

wit_access_token = "GESEUZ4KIUGBAWH65VKENYQITJV5JILD" #Tokenxitประมวลผลข้อความ wit.ai
client = Wit(wit_access_token) #ประมวลผลคำที่ส่งมาว่าอยู่หมวดหมู่ไหน

line_bot_api = LineBotApi(channel_access_token) #อ่านToken Line หน้าบ้านไปหลังบ้าน
handler = WebhookHandler(channel_secret) #อ่านToken Line หลังบ้านไปหน้าบ้าน

app = Flask(__name__)
app.wsgi_app = ProxyFix(app.wsgi_app, x_for=1, x_host=1, x_proto=1)

@app.route("/", methods=["GET","POST"]) #ส่วนที่ตรวจสอบสถานะเซิฟเวอร์ เมื่อระบบทำงานได้จะขึ้นคำว่าHello Line Chatbot
def home():
    try:
        signature = request.headers["X-Line-Signature"]
        body = request.get_data(as_text=True)
        handler.handle(body, signature)
    except:
        pass
    
    return "Hello Line Chatbot"

@handler.add(MessageEvent, message=TextMessage)
def handle_text_message(event):
    text = event.message.text
    print(text)
    
    if (text != ""):
        ret = client.message(text)
        if len(ret["intents"]) > 0:
            confidence = ret["intents"][0]['confidence']
            
            if (confidence > 0.8):
                intents_name = ret["intents"][0]['name']        
                print("intent = ",intents_name)
                #--------------------------------------------------------------------------------copyส่วนนี้
                if (intents_name=="greeting"): #ใส่หมวดหมู่ที่ตั้งไว้
                    text_out = "สวัสดี ยินดีที่ได้รู้จักนะ" #คำตอบ
                    line_bot_api.reply_message(event.reply_token,TextSendMessage(text=text_out))
                #--------------------------------------------------------------------------------copyส่วนนี้
                    
            else:
                print("intent = unknow")
        else:
            print("intent = unknow")


@handler.add(MessageEvent, message=ImageMessage) #ส่วนของข้อความในลักษณะรูปภาพ
def handle_image_message(event):
    message_content = line_bot_api.get_message_content(event.message.id)
    static_tmp_path = os.path.join(os.path.dirname(__file__), 'static', 'tmp').replace("\\","/")
    print(static_tmp_path)
    
    with tempfile.NamedTemporaryFile(dir=static_tmp_path, prefix='jpg' + '-', delete=False) as tf:
        for chunk in message_content.iter_content():
            tf.write(chunk)
        tempfile_path = tf.name
        
    dist_path = tempfile_path + '.jpg'  # เติมนามสกุลเข้าไปในชื่อไฟล์เป็น jpg-xxxxxx.jpg
    os.rename(tempfile_path, dist_path) # เปลี่ยนชื่อไฟล์ภาพเดิมที่ยังไม่มีนามสกุลให้เป็น jpg-xxxxxx.jpg

    filename_image = os.path.basename(dist_path) # ชื่อไฟล์ภาพ output (ชื่อเดียวกับ input)
    filename_fullpath = dist_path.replace("\\","/")   # เปลี่ยนเครื่องหมาย \ เป็น / ใน path เต็ม
    
    img = cv2.imread(filename_fullpath)

    # ใส่โค้ดประมวลผลภาพตรงส่วนนี้
    #-------------------------------------------------------------
    pred_image, obj_box = yolo.predictions(img)
    print(obj_box)
    if len(obj_box) > 0:
        obj_names = ''
        for obj in obj_box:
            obj_names = obj_names + obj[4]
        text_obj = obj_names
    else:
        text_obj = 'ไม่พบวัตถุ'
    #-------------------------------------------------------------
        
    cv2.imwrite(filename_fullpath,pred_image)
    
    
    dip_url = request.host_url + os.path.join('static', 'tmp', filename_image).replace("\\","/")
    print(dip_url)
    
    if text_obj == 'hispa':
        text_out = "ตรวจพบโรคขอบใบแห้ง"
        line_bot_api.reply_message(event.reply_token,[TextSendMessage(text=text_out),
            ImageSendMessage(dip_url,dip_url)])

    if text_obj == 'healthy':
        text_out = "ข้าวสุขภาพดี"
        line_bot_api.reply_message(event.reply_token,[TextSendMessage(text=text_out),
            ImageSendMessage(dip_url,dip_url)])

    if text_obj == 'brownspot':
        text_out = "ตรวจพบโรคใบจุดสีน้ำตาล"
        line_bot_api.reply_message(event.reply_token,[TextSendMessage(text=text_out),
            ImageSendMessage(dip_url,dip_url)])

    if text_obj == 'leafblast':
        text_out = "ตรวจพบโรคใบไหม้"
        line_bot_api.reply_message(event.reply_token,[TextSendMessage(text=text_out),
            ImageSendMessage(dip_url,dip_url)])
    if text_obj == 'ไม่พบวัตถุ':
        text_out = "ไม่สามารถวินิจฉับโรคจากภาพถ่ายได้ ลองเปลี่ยนภาพที่ชัดเจนขึ้น"
        line_bot_api.reply_message(event.reply_token,[TextSendMessage(text=text_out)])
    
@app.route('/static/<path:path>')
def send_static_content(path):
    return send_from_directory('static', path)

if __name__ == "__main__":          
    app.run()

