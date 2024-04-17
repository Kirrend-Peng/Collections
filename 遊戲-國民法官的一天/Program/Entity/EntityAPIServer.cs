using System;
using System.Net.Http;
using System.Threading.Tasks;
using UnityEngine;

public class EntityAPIClient
{
    private static string apiServer_url = "https://aigameapiserver.azurewebsites.net/mediation";

    [System.Serializable]
    class APIServerRequest<T>
    {
        public APIServerHeader Header;
        public T Body;

        public APIServerRequest( T requestObject ) {
            Body = requestObject;
        }
    }

    [System.Serializable]
    class APIServerResponse<T>
    {
        public APIServerHeader Header;
        public T Body;
    }

    [System.Serializable]
    class APIServerHeader
    {
        public string DomainName;
        public string ServiceName;
        public string UserID;

        public APIServerHeader(string domainName, string serviceName)
        {
            DomainName = domainName;
            ServiceName = serviceName;
            UserID = "U1234567890";
        }
    }



    public async Task<string> SendHttpRequestPost(string url, string JsonString)
    {
        using (HttpClient client = new HttpClient())
        {
            var request = new HttpRequestMessage(HttpMethod.Post, url);
            var content = new StringContent(JsonString, null, "text/plain");
            request.Content = content;
            var response = await client.SendAsync(request);
            response.EnsureSuccessStatusCode();
            Debug.Log(await response.Content.ReadAsStringAsync());
            return response.Content.ReadAsStringAsync().Result;
        }
    }

    public async Task<string> SendHttpRequestToAPIServer<T,T2>(T requestObject, string DomainName, string ServiceName)
    {
        using (HttpClient client = new HttpClient())
        {
            APIServerRequest<T> requestJson = new APIServerRequest<T>(requestObject);
            requestJson.Header = new APIServerHeader(DomainName, ServiceName);
            string requestJsonString = JsonUtility.ToJson(requestJson);
            var request = new HttpRequestMessage(HttpMethod.Post, apiServer_url);
            var content = new StringContent(requestJsonString, null, "application/json");
            request.Content = content;
            try
            {
                var response = await client.SendAsync(request).ConfigureAwait(false);
                response.EnsureSuccessStatusCode();
                Debug.Log(await response.Content.ReadAsStringAsync());
                var obj = JsonUtility.FromJson<APIServerResponse<T2>>(response.Content.ReadAsStringAsync().Result);
                return JsonUtility.ToJson(obj.Body);
            }
            catch (Exception ex)
            {
                Debug.Log(ex);
            }

            return null;

        }
    }
}
