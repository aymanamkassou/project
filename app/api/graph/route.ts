export async function GET() {
    try {
      const backendResponse = await fetch('http://35.180.209.123:3001/api/graph');
      const data = await backendResponse.json();
      
      return new Response(JSON.stringify(data), {
        status: 200,
        headers: {
          'Content-Type': 'application/json',
          'Access-Control-Allow-Origin': '*'
        }
      });
    } catch (error) {
      return new Response(JSON.stringify({ error: 'Failed to fetch data' }), {
        status: 500,
        headers: { 'Content-Type': 'application/json' }
      });
    }
  }