from ultralytics import YOLO
import cv2

# @Description: 在图片上调用 YOLO 模型实现目标检测
def yolo_detect_on_image(model_path: str = './model/yolov8n.pt',
                         image_path: str = './images/val/img/test_static_without_person.jpg'):
    model = YOLO(model_path)
    results = model(image_path, imgsz=640)
    results[0].show()

# @Description: 调用摄像头完成实时推理
def yolo_detect_reaction(model_path: str = './model/yolov8n.pt'):
    model = YOLO(model_path)

    cap = cv2.VideoCapture(0)

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        results = model(frame, imgsz=640)
        annotated_frame = results[0].plot()

        cv2.imshow("YOLOv8 Real-Time Detection", annotated_frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

# @Description: 在视频中调用 YOLO 模型实现目标检测
def yolo_detect_on_video(model_path='./model/yolov8n.pt',
                         video_path: str = './images/val/video/learn_process.mp4',
                         display_width: int = 800,
                         display_height: int = 600):
    model = YOLO(model_path)
    cap = cv2.VideoCapture(video_path)
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break

        results = model(frame, imgsz=640)
        annotated_frame = results[0].plot()

        resized_frame = cv2.resize(
            annotated_frame,
            (display_width, display_height),
            interpolation=cv2.INTER_AREA
        )

        cv2.imshow("YOLOv8 Video Detection", resized_frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    cap.release()
    cv2.destroyAllWindows()

if __name__ == '__main__':
    # yolov8n.pt: 无法识别作业本、笔、书籍只有在翻页的时候才可以良好识别, 无法识别电脑键盘、鼠标等器件
    # yolo_detect_on_video(model_path='./model/yolov8n.pt')
    # yolov8s.pt: 无法识别作业本、书籍识别效果不好, 无法识别电脑键盘、鼠标等
    # yolo_detect_on_video(model_path='./model/yolov8s.pt')
    # yolov8m.pt: 卡顿明显、无法识别作业本、笔、书籍等
    # yolo_detect_on_video(model_path='./model/yolov8m.pt')

    # yolov8n-oiv7.pt: 无法识别书本、笔、作业本等 ( 效果很差 )
    # yolo_detect_on_video(model_path='./model/yolov8n-oiv7.pt')
    # yolov8s-oiv7.pt: 无法识别笔记本电脑等 ( 效果差 )
    # yolo_detect_on_video(model_path='./model/yolov8s-oiv7.pt')
    # yolov8m-oiv7.pt: 卡顿严重
    # yolo_detect_on_video(model_path='./model/yolov8m-oiv7.pt')

    # YOLOv8s 测试
    # 手机 ( 正常状态识别比较好 )
    # yolo_detect_on_image('./model/yolov8s.pt', image_path='./images/val/img/cell_phone1.jpg')
    # yolo_detect_on_image('./model/yolov8s.pt', image_path='./images/val/img/cell_phone2.jpg')

    # 平板 ( 无法识别 )
    # yolo_detect_on_image('./model/yolov8s.pt', image_path='./images/val/img/tablet1.jpg')
    # yolo_detect_on_image('./model/yolov8s.pt', image_path='./images/val/img/tablet2.jpg')

    # YOLOv8s 视频测试
    yolo_detect_on_video('./model/yolov8s.pt', video_path='./images/val/video/show_things.mp4')

