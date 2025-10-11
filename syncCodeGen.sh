git add /Users/mt/embedded/uips1
git commit -m "Safe before cubemx sync"

read -p "Продолжить (y/n)? " ans
case "$ans" in
    [Yy]*) 
        cp -r /Users/mt/embedded/uips1/cubeMX_codeGen/Core /Users/mt/embedded/uips1/PIO/cubemx
        cp -r /Users/mt/embedded/uips1/cubeMX_codeGen/Drivers/Users/mt/embedded/uips1/PIO/cubemx
        ;;
    [Nn]*) 
        echo "Отмена."
        exit 0
        ;;
    *) 
        echo "Отмена."
        exit 0
        ;;
esac

