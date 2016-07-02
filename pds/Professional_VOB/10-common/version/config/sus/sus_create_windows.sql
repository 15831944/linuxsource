---------------------------------
--windows version
---------------------------------

--SUS���ݿⴴ��
use master
go

disk init name = "sus_db", physname = "c:\sybase\data\sus.dat", size = "256M"
go

disk init name = "sus_log", physname = "c:\sybase\data\sus.log", size = "128M"
go

disk init name = "sustemp_db", physname = "c:\sybase\data\sustemp.dat", size = "128M"
go

disk init name = "sustemp_log", physname = "c:\sybase\data\sustemp.log", size = "64M"
go

create database sus on sus_db = 256
log on sus_log = 128
go

sp_configure "number of locks", 150000
go

sp_configure 'max memory', 128000 
go
sp_cacheconfig 'default data cache','50M' 
go 
sp_configure 'number of open objects',500
go 
sp_configure 'number of open indexes',500
go 
sp_configure 'procedure cache size',12800
go 

sp_configure "number of user connections", 30
go

sp_dboption sus, "trunc log on chkpt", true 
go 

alter database tempdb on sustemp_db = 128
log on sustemp_log = 64
go

use tempdb
go

sp_dropsegment "default",tempdb,master 
go
sp_dropsegment system,tempdb,master
go
sp_dropsegment logsegment,tempdb,master 
go

use master
go

sp_cacheconfig  "tempdb_cache","2m","mixed"
go

sp_bindcache "tempdb_cache", tempdb
go

sp_dboption sus, "select into/bulkcopy", true
go

use sus 
go

checkpoint
go

setuser 'dbo'
go 

--�û��б�
CREATE TABLE UserTable
(
    UserNo          numeric(9,0)        identity,           --�û���ţ�Ψһ
    UserName        varchar(32)         not null,           --�û�����Ψһ
    Password        char(32)            null,               --�û�����(����MD5����)
    Type            tinyint             not null,           --2-����Ա/1-����Ա
    FullName        varchar(32)         null,               --�����û���
    Description     varchar(64)         null,               --������Ϣ
) with identity_gap = 100
GO

--��־
CREATE TABLE LogTable
(
    SerialNo        numeric(9,0)        identity,           --��־���кţ�Ψһ
    Category        smallint            not null,           --����(EMFileSrvLogReqSubTypeCategory)
    LogLevel        smallint            not null,           --�ȼ�(EMFileSrvLogReqSubTypeLevel)
    Event           smallint            not null,           --�¼�����(EMFileSrvLogReqSubTypeEvent)
    EventSubType    smallint            not null,           --�¼��Ķ���������(EMFileSrvLogReqEventSubTypeDevOpr/ EMFileSrvLogReqEventSubTypeClientOpr)
    Device          varchar(32)         null,               --ת��Ϊ�ַ�������豸��ʶ(IFCSrvDev)
    HardVer         varchar(32)         null,               --Ӳ���汾��
    Time            int                 not null,           --ʱ�����ʱ��(��������ʱ��)
    ErrorCode       int                 not null,           --������
    SpecialInfo     varchar(64)         null                --������Ϣ(�û�����ʱΪ�������û������汾����ʱΪ���º�İ汾��)
) with identity_gap = 100
GO

--�ļ��б�
CREATE TABLE FileTable
(
    FileNo          numeric(9,0)        identity,           --�ļ���ţ�Ψһ
    DevVerNo        int                 not null,           --�豸���
    Filename        varchar(64)         not null,           --�ļ���
    Type            int                 not null,           --�ļ�����(EMFileSrvFileType)
    Size            int                 not null,           --�ļ���С�����ֽڼ���
) with identity_gap = 100
GO

--�豸�б�
CREATE TABLE DevVerTable
(
    DevVerNo        numeric(9,0)        identity,           --�豸�汾��ţ�Ψһ
    DeviceType      varchar(32)         not null,           --ת��Ϊ�ַ�������豸��ʶ(IFCSrvDev)
    HardVer         varchar(32)         not null,           --Ӳ���汾��
    SoftVer         varchar(64)         not null,           --ת��Ϊ�ַ����������汾��(IFCSoftwareVer)
    Report          varchar(512)        null,               --�޸�����
    IsRecommend     tinyint             not null,           --�Ƿ�Ϊ�Ƽ��汾
) with identity_gap = 100
GO



/*
--��������ʱ��ʽ�Ż��������ݷ�ҳ��ѯ����Ĵ洢����
--ע�⣺��Ϊ��ѯ����qrystr�ִ�ӦΪ��select��ѯ
CREATE PROCEDURE splitpageqry
    @qrystr     varchar(5120),
    @startid    int,
    @totalnum   int,
    @userdata   int
AS
    BEGIN
        DECLARE @rcount         int 
        DECLARE @execsql        varchar(5376) 
        DECLARE @tempteablename varchar(256) 

        SELECT @rcount = @startid + @totalnum - 1
        SET rowcount @rcount 
        SET @tempteablename = '#temptable' + convert( varchar, @userdata )
        SET @execsql = stuff( @qrystr, charindex( 'select', @qrystr ), 6,
                              'SELECT sybid = identity(12), ' ) 
        SET @execsql = stuff( @execsql, charindex( 'from', @execsql ), 4,
                              'into ' + @tempteablename + ' from ' ) 
        SET @execsql = @execsql || ' select * from '+ @tempteablename
                       + ' WHERE sybid > ' || convert( varchar, (@startid - 1) ) || ' and sybid <= ' || convert( varchar, ( @startid + @totalnum - 1) ) 
        execute (@execsql)
        SET rowcount 0
    END
GO
*/




--�洢����

--�û�
--��ȡ�û�����
CREATE PROCEDURE user_getcount
AS
    SELECT Count(*) FROM UserTable
GO

--�û���֤
CREATE PROCEDURE user_auth
    @Ret        tinyint OUTPUT,
    @Name       varchar(32),
    @Pwd        varchar(32) = null
AS
    SET @Ret = 3
    DECLARE @nCount tinyint
    SELECT @nCount = Count(*) FROM UserTable WHERE UserName = @Name

    IF ( @nCount = 0 )
        BEGIN
            SET @Ret = 1
        END
    ELSE
        BEGIN--1
            IF ( @Pwd is null or @Pwd = '' )
                BEGIN
                    SELECT @nCount = Count(*) FROM UserTable
                        WHERE UserName = @Name AND (Password = '' or Password is null)
                END
            ELSE
                BEGIN
                    SELECT @nCount = Count(*) FROM UserTable
                        WHERE UserName = @Name AND Password = @Pwd
                END

            IF ( @nCount = 0 )
                BEGIN
                    SET @Ret = 2
                END
            ELSE
                BEGIN
                    SET @Ret = 0
                END
        END--1
GO

--��ѯ�û��б�
CREATE PROCEDURE user_getlist
AS
    SELECT UserNo, UserName, Password, Type, FullName, Description FROM UserTable
GO

--����û�
CREATE PROCEDURE user_add
    @Ret        tinyint OUTPUT,
    @UserNo     numeric(9,0) OUTPUT,
    @Name       varchar(32),
    @Type       tinyint,
    @Pwd        varchar(32) = null,
    @FullName   varchar(32) = null,
    @Desc       varchar(64) = null
AS
    DECLARE @nCount tinyint
    SET @Ret = 2
    SET @UserNo = 0

    SELECT @nCount = COUNT(*) FROM UserTable WHERE UserName = @Name
    IF ( @nCount > 0 )
        BEGIN
            SET @Ret = 1
        END
    ELSE
        BEGIN
            INSERT INTO UserTable ( UserName, Password, Type, FullName, Description )
                VALUES ( @Name, @Pwd, @Type, @FullName, @Desc )
            SET @UserNo = @@identity
            SET @Ret = 0
        END
GO

--�޸��û�
CREATE PROCEDURE user_edit
    @Ret        tinyint OUTPUT,
    @UserNo     numeric(9,0),
    @Name       varchar(32),
    @Type       tinyint,
    @Pwd        varchar(32) = null,
    @FullName   varchar(32) = null,
    @Desc       varchar(64) = null
AS
    DECLARE @nCount tinyint
    DECLARE @OldName varchar(32)

    SET @Ret = 3
    SET @nCount = 0

    SELECT @nCount = COUNT(*) FROM UserTable WHERE UserNo = @UserNo

    IF ( @nCount <> 1 )--ָ���û�������
        BEGIN
            SET @Ret = 1
        END
    ELSE
        BEGIN--1
            SELECT @OldName = UserName FROM UserTable WHERE UserNo = @UserNo

            IF ( @OldName <> @Name )--ָ��λ�õ��û�������������
                BEGIN
                    SET @Ret = 2
                END
            ELSE
                BEGIN
                    UPDATE UserTable
                        SET
                            Password    = @Pwd,
                            Type        = @Type,
                            FullName    = @FullName,
                            Description = @Desc
                        WHERE UserNo = @UserNo
                    SET @Ret = 0
                END
        END--1
GO

--ɾ���û�
CREATE PROCEDURE user_del
    @Ret        tinyint OUTPUT,
    @UserNo     numeric(9,0)
AS
    DECLARE @nCount tinyint
    DECLARE @OldName varchar(32)

    SET @Ret = 3
    SET @nCount = 0

    SELECT @nCount = COUNT(*) FROM UserTable WHERE UserNo = @UserNo

    IF ( @nCount <> 1 )--ָ���û�������
        BEGIN
            SET @Ret = 1
        END
    ELSE
        BEGIN--1
            SELECT @OldName = UserName FROM UserTable WHERE UserNo = @UserNo

            IF ( @OldName = 'admin' )--����ɾ��admin
                BEGIN
                    SET @Ret = 2
                END
            ELSE
                BEGIN
                    DELETE FROM UserTable WHERE UserNo = @UserNo
                    SET @Ret = 0
                END
        END--1
GO


--��־
--����һ����־
CREATE PROCEDURE log_add
    @SerialNo       numeric(9,0) OUTPUT,
    @Category       smallint,
    @Level          smallint,
    @Event          smallint,
    @EventSubType   smallint,
    @Time           int,
    @Device         varchar(32) = null,
    @HardVer        varchar(32) = null,
    @ErrorCode      int = 0,
    @Info           varchar(64) = null
AS
    DECLARE @nCount int
    SELECT @nCount = COUNT(*) FROM LogTable

--    IF ( nCount >= 9999 )--��ౣ��10000��
--        BEGIN
--            
--        END

    INSERT INTO LogTable ( Category, LogLevel, Event, EventSubType, Device,
                           HardVer, Time, ErrorCode, SpecialInfo )
        VALUES ( @Category, @Level, @Event, @EventSubType, @Device, @HardVer,
                 @Time, @ErrorCode, @Info )
    SET @SerialNo = @@identity
GO

--ɾ��һ����־
CREATE PROCEDURE log_del_one
    @SerialNo   numeric(9,0)
AS
    DELETE FROM LogTable WHERE SerialNo = @SerialNo
GO

--�����־
CREATE PROCEDURE log_clear
AS
    DELETE FROM LogTable
GO


--��ѯ�豸�¼���־
CREATE PROCEDURE log_event_dev_query_condition
    @QueryCondition     varchar(3840) OUTPUT,
    @EventDevMask       smallint = null
AS
    DECLARE @DevCondition   varchar(1024)

    IF ( @EventDevMask > 0 )----ָ���豸��־
        BEGIN
            IF ( @EventDevMask >= 4 )--�˳�
                BEGIN
                    SET @EventDevMask = @EventDevMask - 4
                    SET @DevCondition = 'EventSubType = 4'
                END
            IF ( @EventDevMask >= 2 )--��ȡ�ļ�
                BEGIN
                    SET @EventDevMask = @EventDevMask - 2
                    IF ( LEN( @DevCondition ) > 0 )
                        BEGIN
                            SET @DevCondition = @DevCondition + ' OR '
                        END
                    SET @DevCondition = @DevCondition + 'EventSubType = 2'
                END
            IF ( @EventDevMask >= 1 )--��¼
                BEGIN
                    SET @EventDevMask = 0
                    IF ( LEN( @DevCondition ) > 0 )
                        BEGIN
                            SET @DevCondition = @DevCondition + ' OR '
                        END
                    SET @DevCondition = @DevCondition + 'EventSubType = 1'
                END

            SET @QueryCondition = '((Event = 1) AND (' + @DevCondition + '))'
        END
GO

--��ѯ�ͻ����¼���־
CREATE PROCEDURE log_event_client_query_condition
    @QueryCondition     varchar(3840) OUTPUT,
    @EventClientMask    smallint = null
AS
    DECLARE @ClientCondition    varchar(1024)

    IF ( @EventClientMask > 0 )----ָ���豸��־
        BEGIN
            IF ( @EventClientMask >= 128 )--�˳�
                BEGIN
                    SET @EventClientMask = @EventClientMask - 128
                    SET @ClientCondition = 'EventSubType = 128'
                END
            IF ( @EventClientMask >= 64 )--�ϴ��ļ�
                BEGIN
                    SET @EventClientMask = @EventClientMask - 64
                    IF ( LEN( @ClientCondition ) > 0 )
                        BEGIN
                            SET @ClientCondition = @ClientCondition + ' OR '
                        END
                    SET @ClientCondition = @ClientCondition + 'EventSubType = 64'
                END
            IF ( @EventClientMask >= 32 )--ɾ����־
                BEGIN
                    SET @EventClientMask = @EventClientMask - 32
                    IF ( LEN( @ClientCondition ) > 0 )
                        BEGIN
                            SET @ClientCondition = @ClientCondition + ' OR '
                        END
                    SET @ClientCondition = @ClientCondition + 'EventSubType = 32'
                END
            IF ( @EventClientMask >= 16 )--��ѯ��־
                BEGIN
                    SET @EventClientMask = @EventClientMask - 16
                    IF ( LEN( @ClientCondition ) > 0 )
                        BEGIN
                            SET @ClientCondition = @ClientCondition + ' OR '
                        END
                    SET @ClientCondition = @ClientCondition + 'EventSubType = 16'
                END
            IF ( @EventClientMask >= 8 )--ɾ���û�
                BEGIN
                    SET @EventClientMask = @EventClientMask - 8
                    IF ( LEN( @ClientCondition ) > 0 )
                        BEGIN
                            SET @ClientCondition = @ClientCondition + ' OR '
                        END
                    SET @ClientCondition = @ClientCondition + 'EventSubType = 8'
                END
            IF ( @EventClientMask >= 4 )--�޸��û�
                BEGIN
                    SET @EventClientMask = @EventClientMask - 4
                    IF ( LEN( @ClientCondition ) > 0 )
                        BEGIN
                            SET @ClientCondition = @ClientCondition + ' OR '
                        END
                    SET @ClientCondition = @ClientCondition + 'EventSubType = 4'
                END
            IF ( @EventClientMask >= 2 )--����û�
                BEGIN
                    SET @EventClientMask = @EventClientMask - 2
                    IF ( LEN( @ClientCondition ) > 0 )
                        BEGIN
                            SET @ClientCondition = @ClientCondition + ' OR '
                        END
                    SET @ClientCondition = @ClientCondition + 'EventSubType = 2'
                END
            IF ( @EventClientMask >= 1 )--��¼
                BEGIN
                    SET @EventClientMask = 0
                    IF ( LEN( @ClientCondition ) > 0 )
                        BEGIN
                            SET @ClientCondition = @ClientCondition + ' OR '
                        END
                    SET @ClientCondition = @ClientCondition + 'EventSubType = 1'
                END

            SET @QueryCondition = ' ((Event = 2) AND (' + @ClientCondition + '))'
        END
GO

--��ѯ�¼���־
CREATE PROCEDURE log_event_query_condition
    @QueryCondition     varchar(2048) OUTPUT,
    @EventMask          smallint,
    @EventDevMask       smallint = null,
    @EventClientMask    smallint = null
AS
    DECLARE @DevQueryCondition      varchar(1024)
    DECLARE @ClientQueryCondition   varchar(1024)

    IF ( @EventMask >= 2 )--�ͻ��˲���
        BEGIN
            SET @EventMask = @EventMask - 2
            EXEC log_event_dev_query_condition @DevQueryCondition OUTPUT, @EventDevMask
        END

    IF ( @EventMask >= 1 )--�豸����
        BEGIN
            SET @EventMask = 0
            EXEC log_event_client_query_condition @ClientQueryCondition OUTPUT, @EventClientMask
        END

    DECLARE @DevLen     int
    DECLARE @ClientLen  int

    SET @DevLen = LEN( @DevQueryCondition )
    SET @ClientLen = LEN( @ClientQueryCondition )

    --����ַ���֮������ӷ�
    IF ( @DevLen > 0 AND @ClientLen > 0 )
        BEGIN
            SET @QueryCondition = '(' + @DevQueryCondition + ' OR '
                                  + @ClientQueryCondition + ')'
        END
    ELSE IF ( @DevLen > 0 )
        BEGIN
            SET @QueryCondition = @DevQueryCondition
        END
    ELSE IF ( @ClientLen > 0 )
        BEGIN
            SET @QueryCondition = @ClientQueryCondition
        END
GO

--��ѯ�������
CREATE PROCEDURE log_result_query_condition
    @QueryCondition     varchar(2048) OUTPUT,
    @ResultMask         smallint
AS
    DECLARE @TempQuery  varchar(1024)

    IF ( @ResultMask >= 2 )--����ʧ��
        BEGIN
            SET @ResultMask = @ResultMask - 2
            SET @TempQuery = 'ErrorCode <> 0'
        END
    IF ( @ResultMask >= 1 )--�����ɹ�
        BEGIN
            IF ( LEN( @TempQuery ) > 0 )
                BEGIN
                    SET @TempQuery = @TempQuery + ' OR '
                END
            SET @ResultMask = 0
            SET @TempQuery = @TempQuery + 'ErrorCode = 0'
        END

    IF ( LEN( @TempQuery ) > 0 )
        BEGIN
            SET @QueryCondition = '(' + @TempQuery + ')'
        END
GO

--��ѯָ���豸��־
CREATE PROCEDURE log_dev_query_condition
    @QueryCondition varchar(2048) OUTPUT,
    @DevMask        smallint,
    @DevType        varchar(32) = null,
    @HardVer        varchar(32) = null
AS
    DECLARE @TempQuery  varchar(1024)

    IF ( @DevMask >= 8 )--ָ��Ӳ���汾
        BEGIN
            SET @DevMask = @DevMask - 8
            SET @TempQuery = 'HardVer = @HardVer'
        END
    IF ( @DevMask >= 4 )--����ϵͳ
        BEGIN
            IF ( LEN( @TempQuery ) > 0 )
                BEGIN
                    SET @TempQuery = @TempQuery + ' AND '
                END
            SET @DevMask = @DevMask - 4
            SET @TempQuery = @TempQuery + 'substring(Device, 17, 15) = '
                             + substring(@DevType, 17, 15)
        END
    IF ( @DevMask >= 2 )--ָ���ͺ�
        BEGIN
            IF ( LEN( @TempQuery ) > 0 )
                BEGIN
                    SET @TempQuery = @TempQuery + ' AND '
                END
            SET @DevMask = @DevMask - 2
            SET @TempQuery = @TempQuery + 'substring(Device, 9, 8) = '
                             + substring(@DevType, 9, 8)
        END
    IF ( @DevMask >= 1 )--ָ������
        BEGIN
            IF ( LEN( @TempQuery ) > 0 )
                BEGIN
                    SET @TempQuery = @TempQuery + ' AND '
                END
            SET @DevMask = @DevMask - 1
            SET @TempQuery = @TempQuery + 'substring(Device, 1, 8) = '
                             + substring(@DevType, 1, 8)
        END

    IF ( LEN( @TempQuery ) > 0 )
        BEGIN
            SET @QueryCondition = '(' + @TempQuery + ')'
        END
GO

--��ѯʱ��
CREATE PROCEDURE log_time_query_condition
    @QueryCondition varchar(2048) OUTPUT,
    @StartTime      int,
    @EndTime        int
AS
    DECLARE @TempQuery  varchar(1024)

    IF ( @StartTime > 0 )--ָ����ʼʱ��
        BEGIN
            SET @TempQuery = 'Time > @StartTime'
        END
    IF ( @EndTime > 0 )--ָ������ʱ��
        BEGIN
            IF ( LEN( @TempQuery ) > 0 )
                BEGIN
                    SET @TempQuery = @TempQuery + ' AND '
                END
            SET @TempQuery = @TempQuery + 'Time < @EndTime'
        END

    IF ( LEN( @TempQuery ) > 0 )
        BEGIN
            SET @QueryCondition = '(' + @TempQuery + ')'
        END
GO

--��־����
CREATE PROCEDURE log_level_query_condition
    @QueryCondition varchar(2048) OUTPUT,
    @LevelMask      smallint
AS
    DECLARE @TempQuery  varchar(1024)

    IF ( @LevelMask >= 4 )--һ��
        BEGIN
            SET @LevelMask = @LevelMask - 4
            SET @TempQuery = 'LogLevel = 4'
        END
    IF ( @LevelMask >= 2 )--��Ҫ
        BEGIN
            SET @LevelMask = @LevelMask - 2
            IF ( LEN( @TempQuery ) > 0 )
                BEGIN
                    SET @TempQuery = @TempQuery + ' OR '
                END
            SET @TempQuery = @TempQuery + 'LogLevel = 2'
        END
    IF ( @LevelMask >= 1 )--����
        BEGIN
            SET @LevelMask = 0
            IF ( LEN( @TempQuery ) > 0 )
                BEGIN
                    SET @TempQuery = @TempQuery + ' OR '
                END
            SET @TempQuery = @TempQuery + 'LogLevel = 1'
        END

    IF ( LEN( @TempQuery ) > 0 )
        BEGIN
            SET @QueryCondition = '(' + @TempQuery + ')'
        END
GO

--��־����
CREATE PROCEDURE log_category_query_condition
    @QueryCondition varchar(2048) OUTPUT,
    @CategoryMask   smallint
AS
    DECLARE @TempQuery  varchar(1024)

    IF ( @CategoryMask >= 2 )--ϵͳ
        BEGIN
            SET @CategoryMask = @CategoryMask - 2
            SET @TempQuery = 'Category = 2'
        END
    IF ( @CategoryMask >= 1 )--�û�
        BEGIN
            SET @CategoryMask = 0
            IF ( LEN( @TempQuery ) > 0 )
                BEGIN
                    SET @TempQuery = @TempQuery + ' OR '
                END
            SET @TempQuery = @TempQuery + 'Category = 1'
        END

    IF ( LEN( @TempQuery ) > 0 )
        BEGIN
            SET @QueryCondition = '(' + @TempQuery + ')'
        END
GO

--��ѯ��־
CREATE PROCEDURE log_query
    @MainType           tinyint,
    @CategoryMask       smallint = null,
    @LevelMask          smallint = null,
    @ResultMask         smallint = null,
    @DevMask            smallint = null,
    @EventMask          smallint = null,
    @EventDevMask       smallint = null,
    @EventClientMask    smallint = null,
    @DevType            varchar(32) = null,
    @DevHardVer         varchar(32) = null,
    @StartTime          int = null,
    @EndTime            int = null
AS
    IF ( @MainType = 0 )--��ѯ������־
        BEGIN
            SELECT SerialNo, Category, LogLevel, Event, EventSubType,
                   Device, HardVer, Time, ErrorCode, SpecialInfo
                FROM LogTable
        END
    ELSE--�����ѯ
        BEGIN--1
            DECLARE @QueryCondition varchar(3840)
            DECLARE @TempQuery varchar(2048)

            IF ( @MainType >= 32 )--��ѯ�¼�
                BEGIN
                    SET @MainType = @MainType - 32
                    EXEC log_event_query_condition @TempQuery OUTPUT, @EventMask,
                                                   @EventDevMask, @EventClientMask
                    SET @QueryCondition = @QueryCondition + @TempQuery
                END
            IF ( @MainType >= 16 )--�������
                BEGIN
                    SET @MainType = @MainType - 16
                    SET @TempQuery = ''
                    EXEC log_result_query_condition @TempQuery OUTPUT, @ResultMask
                    IF ( LEN( @QueryCondition ) > 0 AND LEN( @TempQuery ) > 0  )
                        BEGIN
                            SET @QueryCondition = @QueryCondition + ' AND '
                        END
                    SET @QueryCondition = @QueryCondition + @TempQuery
                END
            IF ( @MainType >= 8 )--�豸��ʶ
                BEGIN
                    SET @MainType = @MainType - 8
                    SET @TempQuery = ''
                    EXEC log_dev_query_condition @TempQuery OUTPUT, @DevMask, @DevType, @DevHardVer
                    IF ( LEN( @QueryCondition ) > 0 AND LEN( @TempQuery ) > 0  )
                        BEGIN
                            SET @QueryCondition = @QueryCondition + ' AND '
                        END
                    SET @QueryCondition = @QueryCondition + @TempQuery
                END
            IF ( @MainType >= 4 )--ʱ��
                BEGIN
                    SET @MainType = @MainType - 4
                    SET @TempQuery = ''
                    EXEC log_time_query_condition @TempQuery OUTPUT, @StartTime, @EndTime
                    IF ( LEN( @QueryCondition ) > 0 AND LEN( @TempQuery ) > 0  )
                        BEGIN
                            SET @QueryCondition = @QueryCondition + ' AND '
                        END
                    SET @QueryCondition = @QueryCondition + @TempQuery
                END
            IF ( @MainType >= 2 )--��־����
                BEGIN
                    SET @MainType = @MainType - 2
                    SET @TempQuery = ''
                    EXEC log_level_query_condition @TempQuery OUTPUT, @LevelMask
                    IF ( LEN( @QueryCondition ) > 0 AND LEN( @TempQuery ) > 0  )
                        BEGIN
                            SET @QueryCondition = @QueryCondition + ' AND '
                        END
                    SET @QueryCondition = @QueryCondition + @TempQuery
                END
            IF ( @MainType >= 1 )--��־����
                BEGIN
                    SET @MainType = 0
                    SET @TempQuery = ''
                    EXEC log_category_query_condition @TempQuery OUTPUT, @CategoryMask
                    IF ( LEN( @QueryCondition ) > 0 AND LEN( @TempQuery ) > 0  )
                        BEGIN
                            SET @QueryCondition = @QueryCondition + ' AND '
                        END
                    SET @QueryCondition = @QueryCondition + @TempQuery
                END


            DECLARE @Sql varchar(4096)
            SET @Sql = 'SELECT SerialNo, Category, LogLevel, Event, EventSubType, Device, HardVer, Time, ErrorCode, SpecialInfo FROM LogTable WHERE'
                       + @QueryCondition
            EXEC (@Sql)
        END--1
GO

--�豸
--��ѯ�����Ƽ��豸�ļ�
CREATE PROCEDURE dev_getallrecommend_devfile
AS
    SELECT DevVerTable.DevVerNo, DevVerTable.DeviceType, DevVerTable.HardVer,
           DevVerTable.Report, DevVerTable.SoftVer, DevVerTable.IsRecommend,
           FileTable.FileNo, FileTable.Filename, FileTable.Type, FileTable.Size
        FROM DevVerTable INNER JOIN FileTable
        On DevVerTable.IsRecommend = 1
           AND DevVerTable.DevVerNo = FileTable.DevVerNo
GO

--��ѯָ���豸���а汾���ļ�
CREATE PROCEDURE dev_getall_devfile
    @DeviceType varchar(32)
AS
    SELECT DevVerTable.DevVerNo, DevVerTable.DeviceType, DevVerTable.HardVer,
           DevVerTable.Report, DevVerTable.SoftVer, DevVerTable.IsRecommend,
           FileTable.FileNo, FileTable.Filename, FileTable.Type, FileTable.Size
        FROM DevVerTable INNER JOIN FileTable
        On DevVerTable.DeviceType = @DeviceType
           AND DevVerTable.DevVerNo = FileTable.DevVerNo
GO

--�����豸
CREATE PROCEDURE dev_add
    @Ret        tinyint OUTPUT,
    @DevVerNo   numeric(9,0) OUTPUT,
    @DeviceType varchar(32),
    @SoftVer    varchar(64),
    @HardVer    varchar(32) = null,
    @Report     varchar(512) = null
AS
    SET @Ret = 2
    SET @DevVerNo = 0

    DECLARE @nCount int
    SELECT @nCount = COUNT(*) FROM DevVerTable
        WHERE DeviceType = @DeviceType AND SoftVer = @SoftVer

    IF ( @nCount > 0 )--�豸�Ѵ���
        BEGIN
            SET @Ret = 1
        END
    ELSE
        BEGIN
            INSERT INTO DevVerTable ( DeviceType, HardVer, SoftVer, Report, IsRecommend )
                VALUES ( @DeviceType, @HardVer, @SoftVer, @Report, 0 )
            SET @DevVerNo = @@identity
            SET @Ret = 0
        END
GO

--�޸��豸(Ӳ���汾�ŷ�Χ������)
CREATE PROCEDURE dev_edit
    @Ret        tinyint OUTPUT,
    @DevVerNo   numeric(9,0),
    @HardVer    varchar(32),
    @Report     varchar(512) = null
AS
    SET @Ret = 2
    DECLARE @nCount int
    SELECT @nCount = COUNT(*) FROM DevVerTable WHERE DevVerNo = @DevVerNo

    IF ( @nCount = 0 )--�豸������
        BEGIN
            SET @Ret = 1
        END
    ELSE
        BEGIN
            UPDATE DevVerTable
                SET
                    Report      = @Report,
                    HardVer     = @HardVer
                WHERE DevVerNo = @DevVerNo
            SET @Ret = 0
        END
GO

--�޸��Ƽ��汾
CREATE PROCEDURE dev_edit_ver
    @Ret            tinyint         OUTPUT,
    @OldNo          numeric(9,0),
    @DevVerNo       numeric(9,0)    --�Ƽ��汾
AS
    DECLARE @nCount int

    SELECT @nCount = COUNT(*) FROM DevVerTable WHERE DevVerNo = @DevVerNo
    SET @Ret = 2

    IF ( @nCount = 0 )
        BEGIN
            SET @Ret = 1
        END
    ELSE
        BEGIN
            IF ( @OldNo <> 0 )
                BEGIN
                    UPDATE DevVerTable
                        SET
                            IsRecommend = 0 --�ɵ��Ƽ��汾�����Ƽ�
                        WHERE DevVerNo = @OldNo
                END --IF ( @OldNo <> 0 )

            UPDATE DevVerTable
                SET
                    IsRecommend = 1         --�����µ��Ƽ��汾
                WHERE DevVerNo = @DevVerNo
            SET @Ret = 0
        END--IF ( @nCount = 0 )
GO

--��ȡָ���豸���Ƽ��汾
CREATE PROCEDURE dev_get_devfile
    @DevVerNo       numeric(9,0)
AS
    SELECT DevVerTable.DevVerNo, DevVerTable.DeviceType, DevVerTable.HardVer,
           DevVerTable.Report, DevVerTable.SoftVer, DevVerTable.IsRecommend,
           FileTable.FileNo, FileTable.Filename, FileTable.Type, FileTable.Size
        FROM DevVerTable INNER JOIN FileTable
        On DevVerTable.DevVerNo = @DevVerNo AND FileTable.DevVerNo = @DevVerNo
GO


--ɾ���豸�汾
CREATE PROCEDURE dev_del
--    @Ret            tinyint,
    @DevVerNo       numeric(9,0)
AS
--    SET @Ret = 2
--    DECLARE @nCount int
--    SELECT @nCount = COUNT(*) FROM DevVerTable WHERE DevVerNo = @DevVerNo

--    IF ( @nCount = 0 )
--        BEGIN
--            @Ret = 1
--        END
--    ELSE
--        BEGIN
            DELETE FROM FileTable WHERE DevVerNo = @DevVerNo    --ɾ������ļ�
            DELETE FROM DevVerTable WHERE DevVerNo = @DevVerNo  --ɾ���豸
--            @Ret = 0
--        END
GO

--��ȡ����������Ӳ���汾�ŷ�Χ
CREATE PROCEDURE dev_get_hardver
    @DeviceType     varchar(32),            --�豸����
    @SoftVer        varchar(64)             --����汾��
AS
    SELECT HardVer FROM DevVerTable
        WHERE DeviceType = @DeviceType AND SoftVer = @SoftVer
GO

--ɾ�����Ƽ��汾���µķ��Ƽ��汾
--CREATE PROCEDURE dev_del_ver_newer_than_recommend
--    @Device     varchar(32),
--    @SoftVer    varchar(64)     --�Ƽ��汾
--AS
--    DELETE FROM FileTable WHERE
--        DevNo = ( SELECT DevNo FROM DevVerTable WHERE Device = @Device
--                                                   AND SoftVer > @SoftVer )
--    DELETE FROM DevVerTable WHERE Device = @Device AND SoftVer > @SoftVer
--    EXEC dev_edit_ver @Device, @SoftVer
--GO

--�ļ�
--��ѯָ���豸�µ��ļ�
--CREATE PROCEDURE file_query_devfile
--    @DevNo      int
--AS
--    SELECT FileNo, Filename, Type FROM FileTable WHERE DevNo = @DevNo
--GO

--�����ļ�
CREATE PROCEDURE file_add
    @Ret        tinyint OUTPUT,
    @FileNo     numeric(9,0) OUTPUT,
    @DevVerNo   int,
    @Filename   varchar(64),
    @Type       int,
    @Size       int
AS
    SET @Ret = 3
    DECLARE @nCountName int
    DECLARE @nCountType int

    SELECT @nCountName = COUNT(*) FROM FileTable
        WHERE DevVerNo = @DevVerNo AND Filename = @Filename
    SELECT @nCountType = COUNT(*) FROM FileTable
        WHERE DevVerNo = @DevVerNo AND Type = @Type

    IF ( @nCountName > 0 )--�ļ����Ѵ���
        BEGIN
            SET @Ret = 1
        END
    ELSE IF ( @nCountType > 0 )--�ļ������Ѵ���
        BEGIN
            SET @Ret = 2
        END
    ELSE
        BEGIN
            INSERT INTO FileTable ( DevVerNo, Filename, Type, Size )
                VALUES ( @DevVerNo, @Filename, @Type, @Size )
            SET @FileNo = @@identity
            SET @Ret = 0
        END
GO

--�޸��ļ�
--CREATE PROCEDURE file_edit
--    @FileNo     int,
--    @DevNo      int,
--    @Filename   varchar(64),
--    @Type       int
--AS
--    UPDATE FileTable
--        SET
--            Filename    = @Filename,
--            Type        = @Type
--        WHERE FileNo = @FileNo AND DevNo = @DevNo
--GO

--ɾ���ļ�
CREATE PROCEDURE file_del
    @FileNo     numeric(9,0)
AS
    DELETE FROM FileTable WHERE FileNo = @FileNo
GO




--��������
--�û�
--CREATE INDEX UserTable_Index on UserTable(UserNo)

--��־
CREATE INDEX LogTable_Index on LogTable(SerialNo)

--�ļ�
--CREATE INDEX FileTable_Index on FileTable(FileNo)

--�豸
--CREATE INDEX DevVerTable_Index on DevVerTable(DevVerNo)


--�����¼
--admin�û�
INSERT INTO UserTable( UserName, Password, Type, FullName, Description ) VALUES( 'admin', '21232f297a57a5a743894a0e4a801fc', 1, 'administrator', 'administrator' )

--�豸 + �ļ�
--DECLARE @DevNo int
--MT 8220A  Linux
--INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______TS6610__Linux PowerPC__', '9', '40.30.01.01.070123', 'mt 8220A Linux', 1 )
--SET @DevNo = @@identity
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 )
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 )
--MT 8220A VxRaw
--INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______TS6610__Vxworks Raw____', '9', '40.30.01.01.070123', 'mt 8220A VxRaw', 1 )
--SET @DevNo = @@identity
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 )
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 )
--MT 8220B  Linux
--INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______TS5210__Linux PowerPC__', '9', '40.30.01.01.070123', 'mt 8220B Linux', 1 )
--SET @DevNo = @@identity
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 )
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 )
--MT 8220B VxRaw
--INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______TS5210__Vxworks Raw____', '9', '40.30.01.01.070123', 'mt 8220B VxRaw', 1 )
--SET @DevNo = @@identity
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 )
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 )
--MT 8220C  Linux
--INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______V5______Linux PowerPC__', '9', '40.30.01.01.070123', 'mt 8220C Linux', 1 )
--SET @DevNo = @@identity
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 )
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 )
--MT 8220C VxRaw
--INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______V5_____Vxworks Raw____', '9', '40.30.01.01.070123', 'mt 8220C VxRaw', 1 )
--SET @DevNo = @@identity
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 )
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 )
--MT 8620A  Linux
--INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______TS3210__Linux PowerPC__', '9', '40.30.01.01.070123', 'mt 8620A Linux', 1 )
--SET @DevNo = @@identity
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 )
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 )
--MT 8620A VxRaw
--INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______TS3210__Vxworks Raw____', '9', '40.30.01.01.070123', 'mt 8620A VxRaw', 1 )
--SET @DevNo = @@identity
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 )
--INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 )


setuser
go
