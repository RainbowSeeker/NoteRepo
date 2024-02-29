git pull && git add . && echo "git commit -m \"`date +"%Y-%m-%d %H:%M:%S autopush"`\"" | bash  && git push origin -f
read -s -n 1 -p "按任意键退出。。。"