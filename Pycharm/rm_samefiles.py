import os


def deleteDulFile(dirname):
    filenames = [i for i in os.listdir(dirname) if not os.path.isdir(i)]
    for name in filenames:
        splitname = os.path.splitext(name)[0]
        if splitname.endswith('(1)') | splitname.endswith('kgm'):
            print(f'Delete {name}')
            os.remove(os.path.join(dirname,name))
    print(f"clear over in {dirname}")

deleteDulFile('F:\抖音46月')
deleteDulFile('F:\老歌')