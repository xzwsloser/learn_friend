from ultralytics import YOLO

def train_on_gpu():
    model = YOLO("./model/yolov8s.pt")
    results = model.train(
        data='./train.yaml', # 数据集配置文件
        epochs=120, # epochs
        patience=20, # early stop 轮数
        batch=32,  # batch-size
        imgsz=640, # image size
        device=0, # 使用单卡 GPU
        rect=True, # 最小填充策略
        cos_lr=True, # 余弦学习率
        close_mosaic=10,
        amp=True,
        plots=True,
        exists_ok=True
    )

if __name__ == '__main__':
    train_on_gpu()