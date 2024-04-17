[System.Serializable]
public class FC0032001Response
{
    // Start is called before the first frame update
    public string success;
    public Result result;
    public Record records;

    [System.Serializable]
    public class Result
    {
        public string resource_id;
        public Field[] fields;
    }

    [System.Serializable]
    public class Field
    {
        public string id;
        public string type;
    }

    [System.Serializable]
    public class Record
    {
        public string datasetDescription;
        public Location[] location;
    }

    [System.Serializable]
    public class Location
    {
        public string locationName;
        public WeatherElement[] weatherElement;
    }

    [System.Serializable]
    public class WeatherElement
    {
        public string elementName;
        public Time[] time;
    }

    [System.Serializable]
    public class Time
    {
        public string startTime;
        public string endTime;
        public Parameter parameter;
    }

    [System.Serializable]
    public class Parameter
    {
        public string parameterName;
        public string parameterUnit;
        public string parameterValue;
    }

}
