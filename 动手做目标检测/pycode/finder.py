import image

ROIS = [ #[ROI,line,weight]
        (0,200,60,5),
        (0,200,80,5),
        (0,200,100,4),
        (0,200,110,4),
        (0,200,120,3),
        (0,200,130,3),
       ]

#一个简单高效的巡线器
class finder:
    def __init__(self, thresholds, cam_offset, stride=8) -> None:
        if isinstance(thresholds, list) and len(thresholds) == 6:
            pass
        else:
            raise 'thresholds must be a list of size 6'
        self.thresholds = thresholds
        self.cam_offset = cam_offset
        self.stride = stride
        self.error = 0.0
    
    def follow_line(self, img):
        centroid_sum = 0
        is_exist = 0
        for r in ROIS:
            lefborder = r[0]
            rigborder = r[1]
            for l in range(r[0],r[1],self.stride):
                pixel = img.get_pixel(r[2],l)
                lab = image.rgb_to_lab(pixel)
                if lab[0] >= self.thresholds[0] and lab[0] <= self.thresholds[1] and lab[1] >= self.thresholds[2] and lab[1] <= self.thresholds[3] and lab[2] >= self.thresholds[4] and lab[2] <= self.thresholds[5]:
                    img.set_pixel(r[2],l,(0,0,255))
                    if lefborder == 0:
                        lefborder = l
                elif lefborder != 0:
                    rigborder = l
                    is_exist = 1
                    break
            if lefborder != r[0] and rigborder >= lefborder + 55:
                cross_flag = 1;

            centroid_sum += (lefborder + rigborder - r[0] - r[1] + self.cam_offset) * r[3] # r[4] is the roi weight.
        #偏差值
        if is_exist:
            return centroid_sum / 50
        else:
            return 0.0
