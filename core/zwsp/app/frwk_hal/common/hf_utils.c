#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "hf_utils.h"
#include "codestd.h"

/**************************************************************************************************
 * @brief  : 设置文件属性
 * @param  : 属性
 * @param  : 属性值
 * @return : 0成功, 其他失败
 **************************************************************************************************/
int attr_file_set(const char *attr, const char *val)
{
    int fd, ret;

    /* 打开属性 */
    fd = open(attr, O_WRONLY);
    CI_RET(fd < 0, fd, "open %s fail", attr);

    /* 写入属性值 */
    ret = write(fd, val, strlen(val));
    CK_RET(ret < 0, ret);

    /* 关闭属性文件 */
    close(fd);
    return 0;
}