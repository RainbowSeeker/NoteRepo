# Tensor board

### 打开tensorboard

```bash
Tensorboard --logdir=logs --port=8080
```

```python
from torch.utils.tensorboard import SummaryWriter
import numpy as np
from PIL import Image
from  torchvision import transforms

writer = SummaryWriter("logs")

image_path = "cats_dogs_dataset/training_set/training_set/cats/cat.1.jpg"
img = Image.open(image_path)

#方法1：numpy
img_array = np.array(img)
#或者直接cv2读取即可 img_array = cv2.imread(img)
writer.add_image("test",img_array,1,dataformats='HWC')

#方法2：ToTensor
tensor_tran = transforms.ToTensor()
image_tensor = tensor_tran(img)
writer.add_image("test",image_tensor,2)

for i in range(100):
    writer.add_scalar(tag="y=2x",
                      scalar_value=2*i,
                      global_step=i)

writer.close()
```