using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.Data;
using AvauntGuard.RBLStatusCodeGenerator.Lib.DataAccess;
using System.Text.RegularExpressions;

namespace AvauntGuard.RBLStatusCodeGenerator.UI
{
    class Program
    {
        static void Main(string[] args)
        {
            string zoneName = "multi.surbl.org";
            DataSet responses = DataAccessRBL.GetResponses(zoneName, "");

            Recurse(zoneName, responses);
            Console.ReadLine();
        }

        private static void Recurse(string zoneName, DataSet responses)
        {
            Dictionary<string, string> errorCodes = new Dictionary<string, string>();

            if (responses.Tables.Count > 0)
            {
                foreach (DataRow dr in responses.Tables[0].Rows)
                {
                    string key = Regex.Replace(dr["response"].ToString(), "127.0.0.", "");

                    if (!errorCodes.ContainsKey(key))
                        errorCodes.Add(key, dr["reason_id"].ToString());
                }
            }

            int[] items = new int[errorCodes.Count];
            int index = 0;

            foreach (KeyValuePair<string, string> kvp in errorCodes)
                items[index++] = Convert.ToInt32(kvp.Key);

            //Figure out how many bitmasks we need... 
            //4 bits have a maximum value of 15, so we need 15 masks.            
            int len = errorCodes.Count;
            int calcs = (int)Math.Pow(2, len) - 1;

            //Create our array of bitmasks... each item in the array
            //represents a unique combination from our items array
            string[] masks = Enumerable.Range(1, calcs).Select(i => Convert.ToString(i, 2).PadLeft(len, '0')).ToArray();

            //Spit out the corresponding calculation for each bitmask
            foreach (string m in masks)
            {
                //Get the items from our array that correspond to 
                //the on bits in our mask
                int[] incl = items.Where((c, i) => m[i] == '1').ToArray();

                String bases = String.Join(",", incl.Select(c => c.ToString()).ToArray());
                string value = "";

                if (bases.IndexOf(',') >= 0)
                {
                    value = "";

                    foreach (string code in bases.Split(','))
                    {
                        Console.WriteLine(code);

                        if (errorCodes.ContainsKey(code) && value.ToLower().IndexOf(errorCodes[code].ToLower()) < 0)
                            value += errorCodes[code] + ",";
                    }

                    if (value.IndexOf(",") >= 0)
                        value = value.Remove(value.LastIndexOf(","));

                    foreach (string val in value.Split(','))
                    {
                        Console.WriteLine(string.Format("127.0.0.{0} - {1}", incl.Sum(), val));
                        DataAccessRBL.CreateResponse(zoneName, string.Format("127.0.0.{0}", incl.Sum()), Convert.ToInt32(val));
                    }
                }
            }
        }
    }
}
