from ultralytics import YOLO
import os
import cv2
from tqdm import tqdm

# @Description: 在图片上调用 YOLO 模型实现目标检测
def yolo_detect_on_image(model_path: str = './model/yolov8n.pt',
                         image_path: str = './images/val/img/test_static_without_person.jpg',
                         model_name: str = "yolov8s"):
    model = YOLO(model_path)
    results = model(image_path, imgsz=640)
    filename = image_path.split('/')[-1]
    save_dir = f"./images/test/{model_name}"
    os.makedirs(save_dir, exist_ok=True)
    results[0].save(filename=f"{save_dir}/test_{filename}")

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

def yolo_detect_on_test(model_path: str,
                        model_name: str):
    for idx in tqdm(range(1, 60), desc=model_name):
        image_path = f'/home/loser/Desktop/测试集/{idx}.jpg'
        yolo_detect_on_image(model_path=model_path,
                             image_path=image_path,
                             model_name=model_name)


if __name__ == '__main__':
    yolov8s_fine_v2_path = "./runs/detect/yolov8s.pt_fine/weights/best.pt"
    yolov8s_fine_v1_path = "./runs/detect/yolov8s_init/weights/best.pt"
    yolov8n_fine_v2_path = "./runs/detect/yolov8n.pt_fine/weights/best.pt"

    print('-'*10 + 'test yolov8s model fine v2' + '-'*10)
    yolo_detect_on_test(model_path=yolov8s_fine_v2_path,
                        model_name="yolov8s_fine_v2")

    print('-'*10 + 'test yolov8s model fine v1' + '-'*10)
    yolo_detect_on_test(model_path=yolov8s_fine_v1_path,
                        model_name="yolov8s_fine_v1")

    print('-'*10 + 'test yolov8n model fine v2' + '-'*10)
    yolo_detect_on_test(model_path=yolov8n_fine_v2_path,
                        model_name="yolov8n_fine_v2")
