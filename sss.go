package main

import (
    "bufio"
    "context"
    "encoding/csv"
    "errors"
    "fmt"
    "html/template"
    "io"
    "io/ioutil"
    "log"
    "net/http"
    "os"
    "path/filepath"
    "strconv"
    "strings"
    "time"

    L "audio-web-tools/src/lib"

    "github.com/aws/aws-sdk-go-v2/aws"
    "github.com/aws/aws-sdk-go-v2/config"
    "github.com/aws/aws-sdk-go-v2/feature/s3/manager"
    "github.com/aws/aws-sdk-go-v2/service/s3"
    "github.com/google/uuid"
)

type Attributes_struct struct {
    File_name        string
    Codec_long_name  string
    Duration         string
    Sample_rate      string
    Sample_format    string
    Channels         int
    Format_long_name string
}

type PageData struct {
    Buck                string
    Pref                string
    Path                string
    Data                []L.Attributes_struct
    Transcript          []string
    Codec_count         map[string]int
    Sample_rate_count   map[string]int
    Sample_format_count map[string]int
    ErrorVal            error
}

type WordCSVRow struct {
    Word   string
    Exists string
}

type WordPageData struct {
    Path string
    Data []WordCSVRow
}

const MAX_UPLOAD_SIZE = 1024 * 1024 * 50 // 50MB
const PORT_NUM = "7775"

var base_URL string = L.Base_URL
var probe_URL string = L.Probe_URL

/*
Sets up the fileserver, creating handler functions to deal with
different HTML requests from webpages in ./web.
*/
func ServerSetup() {
    fileServer := http.FileServer(http.Dir("./web"))
    http.Handle("/", fileServer)

    http.HandleFunc("/form_check_word", CheckWordFormHandler)

    http.HandleFunc("/upload_attributes", AttributesUploadHandler)
    http.HandleFunc("/upload_transcript", TranscriptUploadHandler)
    http.HandleFunc("/upload_words", CheckWordByFile)

    http.HandleFunc("/transcript_aws", TranscriptByAWS)
    http.HandleFunc("/attributes_aws", AttributesByAWS)

    http.HandleFunc("/attributes_results", AttributesResultsHandler)
    http.HandleFunc("/word_check_results", CheckWordResultsHandler)
    http.HandleFunc("/transcript_results", TranscriptFileResultsHandler)
    http.HandleFunc("/play_audiofile", PlayHandler)

    http.HandleFunc("/waiting_page", WaitingPage)
    http.HandleFunc("/delete_task", DeleteTask)

    if err := http.ListenAndServe(":"+PORT_NUM, nil); err != nil {
        log.Fatal(err)
    }
}

/*
Given a bucket, prefix, and local directory, downloads files from S3 bucket and prefix
to a local directory.

Parameters:
    Bucket: an S3 bucket
    Prefix: an valid S3 prefix existing in Bucket
    LocalDirectory: local directory to store downloaded files
*/
func GetFilesFromS3(Bucket string, Prefix string, LocalDirectory string, maxFiles int, w http.ResponseWriter) error {
    cfg, err := config.LoadDefaultConfig(context.TODO(), config.WithRegion("us-east-1"))
    if err != nil {
        log.Println("error:", err)
    }

    client := s3.NewFromConfig(cfg)
    manager := manager.NewDownloader(client)

    paginator := s3.NewListObjectsV2Paginator(client, &s3.ListObjectsV2Input{
        Bucket: &Bucket,
        Prefix: &Prefix,
    })

    num_files := 0

out:
    for paginator.HasMorePages() {
        page, err := paginator.NextPage(context.TODO())
        if err != nil {
            log.Println("error:", err)
            var PageData PageData

            PageData.ErrorVal = err
            tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
            tmpl.Execute(w, PageData)
            return err
        }
        for _, obj := range page.Contents {
            if err := DownloadToFile(manager, LocalDirectory, Bucket, aws.ToString(obj.Key)); err != nil {
                log.Println("error:", err)
                var PageData PageData

                PageData.ErrorVal = err
                tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
                tmpl.Execute(w, PageData)
                return err
            }
            num_files++
            if num_files >= maxFiles {
                break out
            }
        }
    }
    return nil
}

/*
Helper function to download files from a bucket.

Parameters:
    downloader: an AWS downloader
    targetDirectory: local directory to download files to
    bucket: target S3 bucket to download from
    key: identifier for each file to be downloaded

Return:
    err: any error that occured during download
*/
func DownloadToFile(downloader *manager.Downloader, targetDirectory, bucket, key string) error {
    // Create the directories in the path
    file := filepath.Join(targetDirectory, filepath.Base(key))
    if err := os.MkdirAll(filepath.Dir(file), 0775); err != nil {
        return err
    }

    // Set up the local file
    fd, err := os.Create(file)
    if err != nil {
        return err
    }
    defer fd.Close()

    // Download the file using the AWS SDK for Go
    _, err = downloader.Download(context.TODO(), fd, &s3.GetObjectInput{Bucket: &bucket, Key: &key})

    return err
}

/*
Given an incoming request, will search the lexicon for requested word.

Parameters:
    w: Response Writer to write membership to webpage
    r: incoming request with requested word to search
*/
func CheckWordFormHandler(w http.ResponseWriter, r *http.Request) {

    parseError(w, r)

    word := r.FormValue("word")

    fmt.Fprintf(w, "Searching lexicon for %s . . .\n", word)

    var word_arr []string
    word_arr = append(word_arr, word)
    ret := L.Check_word(word_arr, w)

    if ret[0] {
        fmt.Fprintf(w, "%s exists in the lexicon.", word)
    } else if !ret[0] {
        fmt.Fprintf(w, "%s does NOT exist in the lexicon.", word)
    }
}

var temppathWords string

/*
Given an incoming file, will generate a check words file with membership per word
from the incoming file.

Parameters:
    w: Response Writer
    r: Incoming request
*/
func CheckWordByFile(w http.ResponseWriter, r *http.Request) {
    if r.Method != "POST" {
        err := errors.New("Non POST Method Detected")

        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        return
    }

    file, fileHeader, err := r.FormFile("worduploads")
    if err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    defer file.Close()

    err = os.MkdirAll("./upload_words", os.ModePerm)

    if err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    storageuuid := uuid.New()

    temppathWords = "./upload_words/" + storageuuid.String() + "/"
    tempfile := temppathWords + fileHeader.Filename

    if _, mkerr := os.Stat(temppathWords); os.IsNotExist(mkerr) {
        os.MkdirAll(temppathWords, 0760) // Create your file
    }

    audiof, err := os.OpenFile(tempfile, os.O_RDONLY|os.O_WRONLY|os.O_CREATE, 0760)
    if err != nil {
        os.RemoveAll(temppathWords)
        fmt.Println("Panic in file creation")
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    if _, err = io.Copy(audiof, file); err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    readFile, err := os.Open(tempfile)
    if err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    fileScanner := bufio.NewScanner(readFile)
    fileScanner.Split(bufio.ScanLines)

    //record storage for words and bools
    records := [][]string{
        {"word", "exists"},
    }

    //create file to be returned
    returnFileName := temppathWords + "word_check_results.csv"
    fRet, err := os.Create(returnFileName)
    if err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    defer fRet.Close()

    var readLines []string
    for fileScanner.Scan() {
        readLines = append(readLines, fileScanner.Text())
    }

    exists := L.Check_word(readLines, w) // bool data to check all words

    readFile, err = os.Open(tempfile)
    if err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    i := 0
    fileScanner2 := bufio.NewScanner(readFile)
    fileScanner2.Split(bufio.ScanLines)

    var WordPageData WordPageData
    //build .csv data
    for fileScanner2.Scan() {
        row := []string{fileScanner2.Text(), strconv.FormatBool(exists[i])}
        records = append(records, row)

        WordPageData.Data = append(WordPageData.Data, WordCSVRow{row[0], row[1]})
        i++
    }
    WordPageData.Path = "/word_check_results?uuid=" + storageuuid.String()
    writer := csv.NewWriter(fRet)

    err = writer.WriteAll(records)
    if err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    //set a 15 minute timer to delete directory
    time.AfterFunc(15*time.Minute, func() { os.RemoveAll(temppathWords) })

    //put file up on website for download
    tmpl := template.Must(template.ParseFiles("web/responses/word_response.html"))

    tmpl.Execute(w, WordPageData)
}

var temppathattributes string

/*
Given incoming file(s), will generate a file with relevant metadata per input file.

Parameters:
    w: Response Writer
    r: Incoming request
*/
func AttributesUploadHandler(w http.ResponseWriter, r *http.Request) {
    if r.Method != "POST" {
        err := errors.New("Non POST Method Detected")

        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    r.Body = http.MaxBytesReader(w, r.Body, MAX_UPLOAD_SIZE)
    if err := r.ParseMultipartForm(MAX_UPLOAD_SIZE); err != nil {
        err := errors.New("Please choose a file less than 50 MB in size")

        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    formdata := r.MultipartForm

    files := formdata.File["multiplefiles"]

    storageuuid := uuid.New()

    temppathattributes = "./upload_attributes/" + storageuuid.String() + "/"
    if _, mkerr := os.Stat(temppathattributes); os.IsNotExist(mkerr) {
        os.MkdirAll(temppathattributes, 0760) // Create your file
    }

    returnfile := "results.txt"
    f, err := os.Create(temppathattributes + returnfile)
    if err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    var PageData PageData

    //for each file, store it, analyze it, and append the analysis to return file.
    for i := range files {
        file, err := files[i].Open()
        if err != nil {
            PageData.ErrorVal = err
            tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
            tmpl.Execute(w, PageData)
            log.Println(err)
            return
        }

        defer file.Close()

        tempfile := temppathattributes + files[i].Filename

        if _, mkerr := os.Stat(temppathattributes); os.IsNotExist(mkerr) {
            os.MkdirAll(temppathattributes, 0760) // Create your file
        }

        audiof, err := os.OpenFile(tempfile, os.O_RDONLY|os.O_WRONLY|os.O_CREATE, 0760)
        if err != nil {
            os.RemoveAll(temppathattributes)
            fmt.Println("Panic in file creation")

            PageData.ErrorVal = err
            tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
            tmpl.Execute(w, PageData)
            log.Println(err)
            return
        }

        if _, err = io.Copy(audiof, file); err != nil {
            PageData.ErrorVal = err
            tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
            tmpl.Execute(w, PageData)
            log.Println(err)
            return
        }

        data := L.GetProbe(tempfile, w) //retrieve data per file

        //write data per uploaded file to return file
        _, err = f.WriteString(fmt.Sprintf("File Name: %s\n", data.File_name))
        _, err = f.WriteString(fmt.Sprintf("CODEC Long Name: %s\n", data.Codec_long_name))
        _, err = f.WriteString(fmt.Sprintf("Duration (sec): %s\n", data.Duration))
        _, err = f.WriteString(fmt.Sprintf("Sample Rate (Hz): %s\n", data.Sample_rate))
        _, err = f.WriteString(fmt.Sprintf("Sample Format: %s\n", data.Sample_format))
        _, err = f.WriteString(fmt.Sprintf("Channels: %s\n", data.Channels))
        _, err = f.WriteString(fmt.Sprintf("Format Long Name: %s\n\n", data.Format_long_name))

        PageData.Data = append(PageData.Data, data)
    }
    //prepare quick summary stats
    Codec_count := make(map[string]int)
    Sample_format_count := make(map[string]int)
    Sample_rate_count := make(map[string]int)

    for _, fileAttr := range PageData.Data {
        Codec_count[fileAttr.Codec_long_name]++
        Sample_format_count[fileAttr.Sample_format]++
        Sample_rate_count[fileAttr.Sample_rate]++
    }

    PageData.Codec_count = Codec_count
    PageData.Sample_format_count = Sample_format_count
    PageData.Sample_rate_count = Sample_rate_count

    PageData.Path = "/attributes_results?uuid=" + storageuuid.String()
    defer f.Close()

    //set a 15 minute timer to delete directory
    time.AfterFunc(15*time.Minute, func() { os.RemoveAll(temppathattributes) })

    //put return file up on website for download and display data as plaintext
    tmpl := template.Must(template.ParseFiles("web/responses/attributes_response.html"))

    tmpl.Execute(w, PageData)
}

/*
Given incoming S3 bucket and prefix, will generate a file with relevant metadata per input file.

Parameters:
    w: Response Writer
    r: Incoming request
*/
func AttributesByAWS(w http.ResponseWriter, r *http.Request) {
    parseError(w, r)

    bucket := r.FormValue("bucket")
    prefix := r.FormValue("prefix")

    if bucket == "" || prefix == "" {
        var PageData PageData

        err := errors.New("empty bucket or prefix value")
        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("../web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
    }

    storageuuid := uuid.New()
    temppath := "./upload_attributes/" + storageuuid.String() + "/"

    err := GetFilesFromS3(bucket, prefix, temppath, 50, w)

    if err != nil {
        return
    }
    var PageData PageData

    items, _ := ioutil.ReadDir(temppath)
    PageData.Buck = bucket
    PageData.Pref = prefix

    //make return file
    returnfile := "results.txt"
    f, err := os.Create(temppath + returnfile)
    if err != nil {
        log.Println(err)

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    //for each file, append the metadata
    for _, item := range items {
        data := L.GetProbe(temppath+item.Name(), w) //retrieve data per file

        //write data per uploaded file to return file
        _, err = f.WriteString(fmt.Sprintf("File Name: %s\n", data.File_name))
        _, err = f.WriteString(fmt.Sprintf("CODEC Long Name: %s\n", data.Codec_long_name))
        _, err = f.WriteString(fmt.Sprintf("Duration (sec): %s\n", data.Duration))
        _, err = f.WriteString(fmt.Sprintf("Sample Rate (Hz): %s\n", data.Sample_rate))
        _, err = f.WriteString(fmt.Sprintf("Sample Format: %s\n", data.Sample_format))
        _, err = f.WriteString(fmt.Sprintf("Channels: %s\n", data.Channels))
        _, err = f.WriteString(fmt.Sprintf("Format Long Name: %s\n\n", data.Format_long_name))

        PageData.Data = append(PageData.Data, data)
    }

    //prepare quick summary stats
    Codec_count := make(map[string]int)
    Sample_format_count := make(map[string]int)
    Sample_rate_count := make(map[string]int)

    for _, fileAttr := range PageData.Data {
        Codec_count[fileAttr.Codec_long_name]++
        Sample_format_count[fileAttr.Sample_format]++
        Sample_rate_count[fileAttr.Sample_rate]++
    }

    PageData.Codec_count = Codec_count
    PageData.Sample_format_count = Sample_format_count
    PageData.Sample_rate_count = Sample_rate_count

    defer f.Close()

    //set a 15 minute timer to delete directory
    time.AfterFunc(15*time.Minute, func() { os.RemoveAll(temppath) })

    PageData.Path = "/attributes_results?uuid=" + storageuuid.String()
    //put return file up on website for download and display data as plaintext
    tmpl := template.Must(template.ParseFiles("web/responses/attributes_response.html"))

    tmpl.Execute(w, PageData)
}

var temppathTranscript string
var tempfileName string
var Transcriptuuid uuid.UUID

/*
Given incoming file(s), will generate a transcript per file.
Transcripts are stored in one output file with hyphen delimiters for separation.

Parameters:
    w: Response Writer
    r: Incoming request
*/
func TranscriptUploadHandler(w http.ResponseWriter, r *http.Request) {

    if r.Method != "POST" {
        err := errors.New("Non POST Method Detected")

        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    r.Body = http.MaxBytesReader(w, r.Body, MAX_UPLOAD_SIZE)
    if err := r.ParseMultipartForm(MAX_UPLOAD_SIZE); err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    formdata := r.MultipartForm

    files := formdata.File["multiplefiles"]

    Transcriptuuid = uuid.New()

    //upload each file to the upload_transcript directory
    for i := range files {

        file, err := files[i].Open()
        if err != nil {
            var PageData PageData

            PageData.ErrorVal = err
            tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
            tmpl.Execute(w, PageData)
            log.Println(err)
            return
        }

        defer file.Close()

        temppathTranscript = "./upload_transcript/" + Transcriptuuid.String() + "/"
        tempfileName = temppathTranscript + files[i].Filename

        if _, mkerr := os.Stat(temppathTranscript); os.IsNotExist(mkerr) {
            os.MkdirAll(temppathTranscript, 0760) // Create your file
        }

        audiof, err := os.OpenFile(tempfileName, os.O_RDONLY|os.O_WRONLY|os.O_CREATE, 0760)
        if err != nil {
            os.RemoveAll(temppathTranscript)
            log.Println("Panic in file creation")

            var PageData PageData

            PageData.ErrorVal = err
            tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
            tmpl.Execute(w, PageData)
            log.Println(err)
            return
        }

        if _, err = io.Copy(audiof, file); err != nil {
            var PageData PageData

            PageData.ErrorVal = err
            tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
            tmpl.Execute(w, PageData)
            log.Println(err)
            return
        }
    }

    //execute the waiting page
    tmpl := template.Must(template.ParseFiles("web/waiting_page/transcript_wait.html"))
    pagedata := PageData{}

    tmpl.Execute(w, pagedata)
}

/*
Given incoming S3 bucket and prefix, will generate a transcript per file.
Transcripts are stored in one output file with hyphen delimiters for separation.
For now, only single file uploads are supported.

Parameters:
    w: Response Writer
    r: Incoming request
*/
func TranscriptByAWS(w http.ResponseWriter, r *http.Request) {
    parseError(w, r)

    bucket := r.FormValue("bucket")
    prefix := r.FormValue("prefix")

    if bucket == "" || prefix == "" {
        var PageData PageData

        err := errors.New("empty bucket or prefix value")
        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("../web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
    }

    Transcriptuuid = uuid.New()
    temppathTranscript = "./upload_transcript/" + Transcriptuuid.String() + "/"

    err := GetFilesFromS3(bucket, prefix, temppathTranscript, 1, w)

    if err != nil {
        return
    }
    //get full path to access files
    wd, err := os.Getwd()
    if err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        return
    }
    parent := filepath.Dir(wd)

    //open directory
    file, err := os.Open(parent + "/src/upload_transcript/" + Transcriptuuid.String() + "/")
    if err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        return
    }
    defer file.Close()

    filelist, _ := file.Readdirnames(0)
    tempfileName = filelist[0]

    //execute waiting page
    tmpl := template.Must(template.ParseFiles("web/waiting_page/transcript_wait.html"))
    pagedata := PageData{}

    tmpl.Execute(w, pagedata)
}

/*
Runs Transcription task on the HTML waiting page web/waiting_page/transcript_wait.html.

Parameters:
    w: Response Writer
    r: Incoming request
*/
func WaitingPage(w http.ResponseWriter, r *http.Request) {

    Transcript := L.Return_transcript(tempfileName, temppathTranscript, Transcriptuuid.String(), w)
    Transcript = strings.Replace(Transcript, "\t", "--------", -1)
    res := strings.Split(Transcript, "\n")

    tmpl := template.Must(template.ParseFiles("web/responses/transcript_response.html"))
    pagedata := PageData{
        Path:       "/transcript_results?uuid=" + Transcriptuuid.String(),
        Transcript: res,
    }
    tmpl.Execute(w, pagedata)

    return
}

/*
Deletes a queued task to NTE.

Parameters:
    w: Response Writer
    r: Incoming request
*/
func DeleteTask(w http.ResponseWriter, r *http.Request) {
    client := &http.Client{}

    // Create request
    req, err := http.NewRequest("DELETE", base_URL+"/jobs/"+L.RefID.Reference, nil)
    if err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    // Fetch Request
    resp, err := client.Do(req)
    if err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }
    defer resp.Body.Close()

    // Read Response Body
    respBody, err := ioutil.ReadAll(resp.Body)
    if err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
        return
    }

    fmt.Fprintf(w, "Job Terminated: %s", string(respBody))

    os.RemoveAll(temppathTranscript)

    return
}

/*
Writes the data from the generated file to the webpage in the web/responses directory.
The user can download the file from the webpage.

Parameters:
    w: Response Writer
    r: Incoming request
*/
func CheckWordResultsHandler(w http.ResponseWriter, r *http.Request) {
    incoming_uuid := r.URL.Query()["uuid"]

    Results_data, err := ioutil.ReadFile("upload_words/" + incoming_uuid[0] + "/word_check_results.csv")

    if err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
    }

    w.Write((Results_data))

    os.RemoveAll("upload_words/" + incoming_uuid[0])
    return
}

/*
Play the audio file

Parameters:
    w: Response Writer
    r: Incoming request
*/
func PlayHandler(w http.ResponseWriter, r *http.Request) {

    incoming_uuid := r.URL.Query()["uuid"]

    Results_data, err := ioutil.ReadFile("upload_attributes/" + incoming_uuid[0] + "/results.txt")

    if err != nil {
        panic(err)
    }

    w.Write((Results_data))

    os.RemoveAll("upload_attributes/" + incoming_uuid[0])
    return
}

/*
Writes the data from the generated file to the webpage in the web/responses directory.
The user can download the file from the webpage.

Parameters:
    w: Response Writer
    r: Incoming request
*/
func AttributesResultsHandler(w http.ResponseWriter, r *http.Request) {

    incoming_uuid := r.URL.Query()["uuid"]

    Results_data, err := ioutil.ReadFile("upload_attributes/" + incoming_uuid[0] + "/results.txt")

    if err != nil {
        panic(err)
    }

    w.Write((Results_data))

    os.RemoveAll("upload_attributes/" + incoming_uuid[0])
    return
}

/*
Writes the data from the generated file to the webpage in the web/responses directory.
The user can download the file from the webpage.

Parameters:
    w: Response Writer
    r: Incoming request
*/
func TranscriptFileResultsHandler(w http.ResponseWriter, r *http.Request) {
    incoming_uuid := r.URL.Query()["uuid"]

    Results_data, err := ioutil.ReadFile("upload_transcript/" + incoming_uuid[0] + "/transcript.txt")

    if err != nil {
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        log.Println(err)
    }

    w.Write((Results_data))

    os.RemoveAll(temppathTranscript)
    return
}

/*
Returns a parse error.

Parameters:
    w: Response Writer
    r: Incoming request
*/
func parseError(w http.ResponseWriter, r *http.Request) {
    if err := r.ParseForm(); err != nil {
        fmt.Fprintf(w, "ParseForm() err: %v", err)
        var PageData PageData

        PageData.ErrorVal = err
        tmpl := template.Must(template.ParseFiles("web/responses/errors.html"))
        tmpl.Execute(w, PageData)
        return
    }
}

func main() {
    fmt.Printf("Starting server at port " + PORT_NUM + "\n")

    resp, err := L.GetStatus(L.Probe_URL)
    if err != nil || resp != "Service is up" {
        L.Probe_URL = "http://localhost:7777"
        L.Base_URL = "http://localhost:6000/v4"
    }

    ServerSetup()
}