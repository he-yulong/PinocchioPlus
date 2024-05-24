import numpy as np
from scipy.spatial.transform import Rotation as R

offset = R.from_euler('Y', 1)
def process(data):
    processed_data = []
    first_channels = None
    for t, line in enumerate(data):
        channels = [float(c) for c in line.split()]
        if first_channels is None:
            first_channels = channels
        for joint_num in range(len(channels) // 6):
            if joint_num == 0:
                channels[joint_num * 6 + 3:joint_num * 6 + 6] = [first_channels[3], first_channels[4], first_channels[5]]
            if joint_num == 13:
                slice = channels[joint_num * 6:joint_num * 6 + 3]
                # print(slice)
                r = R.from_rotvec(slice)
                #process rotation
                offset = R.from_euler('Z', t * 1.2, degrees=True)
                # r = offset * r
                r = offset
                channels[joint_num * 6:joint_num * 6 + 3] = r.as_rotvec()
            else:
                r = R.from_euler('x', 0, degrees=True)
                channels[joint_num * 6:joint_num * 6 + 3] = r.as_rotvec()
        # print(r.as_rotvec())
        processed_line = ' '.join([str(c) for c in channels]) + '\n'
        processed_data.append(processed_line)
    return processed_data


if __name__ == '__main__':
    filename = 'walk'
    with open(filename + '.txt', 'r') as f:
        data = f.readlines()

    print(data[0])
    channel_num = len(data[0].split())
    print(channel_num / 6)
    processed_data = process(data)
    with open(filename + '_made_by_yulong.txt', 'w') as f:
        for line in processed_data:
            f.write(line)
